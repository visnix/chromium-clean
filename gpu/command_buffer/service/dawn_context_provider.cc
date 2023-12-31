// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/dawn_context_provider.h"

#include <memory>
#include <vector>

#include "base/check_op.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/notreached.h"
#include "base/threading/platform_thread.h"
#include "base/trace_event/trace_arguments.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "gpu/command_buffer/service/dawn_instance.h"
#include "gpu/command_buffer/service/dawn_platform.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/config/gpu_preferences.h"
#include "third_party/skia/include/gpu/graphite/Context.h"
#include "third_party/skia/include/gpu/graphite/dawn/DawnBackendContext.h"
#include "third_party/skia/include/gpu/graphite/dawn/DawnUtils.h"

#if BUILDFLAG(IS_WIN)
#include "third_party/dawn/include/dawn/native/D3D11Backend.h"
#include "ui/gl/gl_angle_util_win.h"
#endif

namespace gpu {
namespace {

void LogInfo(WGPULoggingType type, char const* message, void* userdata) {
  VLOG(1) << message;
}

void LogError(WGPUErrorType type, char const* message, void* userdata) {
  LOG(ERROR) << message;
}

void LogDeviceLost(WGPUDeviceLostReason reason,
                   char const* message,
                   void* userdata) {
  if (reason == WGPUDeviceLostReason::WGPUDeviceLostReason_Destroyed)
    return;
  LOG(FATAL) << message;
}

wgpu::BackendType GetDefaultBackendType() {
#if BUILDFLAG(IS_WIN)
  return base::FeatureList::IsEnabled(features::kSkiaGraphiteDawnUseD3D12)
             ? wgpu::BackendType::D3D12
             : wgpu::BackendType::D3D11;
#elif BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
  return wgpu::BackendType::Vulkan;
#elif BUILDFLAG(IS_APPLE)
  return wgpu::BackendType::Metal;
#else
  NOTREACHED();
  return wgpu::BackendType::Null;
#endif
}

class Platform : public webgpu::DawnPlatform {
 public:
  using webgpu::DawnPlatform::DawnPlatform;

  ~Platform() override = default;

  const unsigned char* GetTraceCategoryEnabledFlag(
      dawn::platform::TraceCategory category) override {
    return TRACE_EVENT_API_GET_CATEGORY_GROUP_ENABLED(
        TRACE_DISABLED_BY_DEFAULT("gpu.graphite.dawn"));
  }
};

#if BUILDFLAG(IS_WIN)
bool GetANGLED3D11DeviceLUID(LUID* luid) {
  // On Windows, query the LUID of ANGLE's adapter.
  Microsoft::WRL::ComPtr<ID3D11Device> d3d11_device =
      gl::QueryD3D11DeviceObjectFromANGLE();
  if (!d3d11_device) {
    LOG(ERROR) << "Failed to query ID3D11Device from ANGLE.";
    return false;
  }

  Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
  if (!SUCCEEDED(d3d11_device.As(&dxgi_device))) {
    LOG(ERROR) << "Failed to get IDXGIDevice from ANGLE.";
    return false;
  }

  Microsoft::WRL::ComPtr<IDXGIAdapter> dxgi_adapter;
  if (!SUCCEEDED(dxgi_device->GetAdapter(&dxgi_adapter))) {
    LOG(ERROR) << "Failed to get IDXGIAdapter from ANGLE.";
    return false;
  }

  DXGI_ADAPTER_DESC adapter_desc;
  if (!SUCCEEDED(dxgi_adapter->GetDesc(&adapter_desc))) {
    LOG(ERROR) << "Failed to get DXGI_ADAPTER_DESC from ANGLE.";
    return false;
  }

  *luid = adapter_desc.AdapterLuid;
  return true;
}
#endif  // BUILDFLAG(IS_WIN)

}  // namespace

std::unique_ptr<DawnContextProvider> DawnContextProvider::Create(
    webgpu::DawnCachingInterfaceFactory* caching_interface_factory,
    CacheBlobCallback callback) {
  auto context_provider =
      base::WrapUnique(new DawnContextProvider(caching_interface_factory));

  // TODO(rivr): This may return a GPU that is not the active one. Currently
  // the only known way to avoid this is platform-specific; e.g. on Mac, create
  // a Dawn device, get the actual Metal device from it, and compare against
  // MTLCreateSystemDefaultDevice().
  if (!context_provider->Initialize(std::move(callback))) {
    context_provider.reset();
  }
  return context_provider;
}

DawnContextProvider::DawnContextProvider(
    webgpu::DawnCachingInterfaceFactory* caching_interface_factory)
    : caching_interface_factory_(caching_interface_factory) {}
DawnContextProvider::~DawnContextProvider() = default;

bool DawnContextProvider::Initialize(CacheBlobCallback callback) {
  std::unique_ptr<webgpu::DawnCachingInterface> caching_interface;
  if (caching_interface_factory_) {
    caching_interface = caching_interface_factory_->CreateInstance(
        kGraphiteDawnGpuDiskCacheHandle, std::move(callback));
  }

  platform_ = std::make_unique<Platform>(std::move(caching_interface));

  GpuPreferences preferences;
#if DCHECK_IS_ON()
  preferences.enable_dawn_backend_validation =
      DawnBackendValidationLevel::kFull;
#else
  preferences.enable_dawn_backend_validation =
      DawnBackendValidationLevel::kDisabled;
#endif

  instance_ = webgpu::DawnInstance::Create(platform_.get(), preferences);

  // If a new toggle is added here, ForceDawnTogglesForSkia() which collects
  // info for about:gpu should be updated as well.
  wgpu::DawnTogglesDescriptor toggles_desc;
  std::vector<const char*> enabled_toggles;
#if DCHECK_IS_ON()
  enabled_toggles.push_back("use_user_defined_labels_in_backend");
#else
  // Disable validation in non-DCHECK builds.
  // TODO(crbug.com/1456492): check if below toggles are necessary.
  enabled_toggles.push_back("disable_robustness");
  enabled_toggles.push_back("skip_validation");
#endif
  toggles_desc.enabledToggles = enabled_toggles.data();
  toggles_desc.enabledTogglesCount = enabled_toggles.size();

  wgpu::DeviceDescriptor descriptor;
  descriptor.nextInChain = &toggles_desc;

  // TODO(crbug.com/1456492): verify the required features.
  std::vector<wgpu::FeatureName> features = {
      wgpu::FeatureName::DawnInternalUsages,
      wgpu::FeatureName::DawnMultiPlanarFormats,
      wgpu::FeatureName::ImplicitDeviceSynchronization,
      wgpu::FeatureName::SurfaceCapabilities,
  };

  wgpu::RequestAdapterOptions adapter_options;
  adapter_options.backendType = GetDefaultBackendType();
  adapter_options.powerPreference = wgpu::PowerPreference::LowPower;

#if BUILDFLAG(IS_WIN)
  if (adapter_options.backendType == wgpu::BackendType::D3D11) {
    features.push_back(wgpu::FeatureName::D3D11MultithreadProtected);
  }

  // Request the GPU that ANGLE is using if possible.
  dawn::native::d3d::RequestAdapterOptionsLUID adapter_options_luid;
  if (GetANGLED3D11DeviceLUID(&adapter_options_luid.adapterLUID)) {
    adapter_options.nextInChain = &adapter_options_luid;
  }
#endif  // BUILDFLAG(IS_WIN)

  std::vector<dawn::native::Adapter> adapters =
      instance_->EnumerateAdapters(&adapter_options);
  if (adapters.empty()) {
    LOG(ERROR) << "No adapters found.";
    return false;
  }

  wgpu::Adapter adapter(adapters[0].Get());
  if (adapter.HasFeature(wgpu::FeatureName::TransientAttachments)) {
    features.push_back(wgpu::FeatureName::TransientAttachments);
  }

  descriptor.requiredFeatures = features.data();
  descriptor.requiredFeaturesCount = std::size(features);

  wgpu::Device device = adapter.CreateDevice(&descriptor);
  if (!device) {
    LOG(ERROR) << "Failed to create device.";
    return false;
  }

  device.SetUncapturedErrorCallback(&LogError, nullptr);
  device.SetDeviceLostCallback(&LogDeviceLost, nullptr);
  device.SetLoggingCallback(&LogInfo, nullptr);
  device_ = std::move(device);

  return true;
}

bool DawnContextProvider::InitializeGraphiteContext(
    const skgpu::graphite::ContextOptions& options) {
  CHECK(!graphite_context_);

  if (device_) {
    skgpu::graphite::DawnBackendContext backend_context;
    backend_context.fDevice = device_;
    backend_context.fQueue = device_.GetQueue();
    graphite_context_ =
        skgpu::graphite::ContextFactory::MakeDawn(backend_context, options);
  }

  return !!graphite_context_;
}

wgpu::Instance DawnContextProvider::GetInstance() const {
  return instance_->Get();
}

#if BUILDFLAG(IS_WIN)
Microsoft::WRL::ComPtr<ID3D11Device> DawnContextProvider::GetD3D11Device()
    const {
  if (GetDefaultBackendType() == wgpu::BackendType::D3D11) {
    return dawn::native::d3d11::GetD3D11Device(device_.Get());
  }
  return nullptr;
}
#endif

}  // namespace gpu
