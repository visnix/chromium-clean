// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ash/nearby/quick_start_connectivity_service.h"

#include "base/memory/weak_ptr.h"
#include "chrome/browser/nearby_sharing/nearby_connections_manager_impl.h"
#include "chrome/browser/nearby_sharing/public/cpp/nearby_connections_manager.h"
#include "chromeos/ash/services/nearby/public/cpp/nearby_process_manager.h"

namespace ash::quick_start {

namespace {

constexpr char kServiceId[] = "com.google.android.gms.smartdevice.NC_ID";

}  // namespace

QuickStartConnectivityService::QuickStartConnectivityService(
    nearby::NearbyProcessManager* nearby_process_manager)
    : nearby_process_manager_(nearby_process_manager) {}

QuickStartConnectivityService::~QuickStartConnectivityService() = default;

base::WeakPtr<NearbyConnectionsManager>
QuickStartConnectivityService::GetNearbyConnectionsManager() {
  DCHECK(nearby_process_manager_);

  if (!nearby_connections_manager_) {
    nearby_connections_manager_ =
        std::make_unique<NearbyConnectionsManagerImpl>(nearby_process_manager_,
                                                       kServiceId);
  }

  return nearby_connections_manager_->GetWeakPtr();
}

mojo::SharedRemote<mojom::QuickStartDecoder>
QuickStartConnectivityService::GetQuickStartDecoder() {
  DCHECK(nearby_process_manager_);

  if (!nearby_process_reference_) {
    nearby_process_reference_ =
        nearby_process_manager_->GetNearbyProcessReference(base::BindOnce(
            &QuickStartConnectivityService::OnNearbyProcessStopped,
            weak_ptr_factory_.GetWeakPtr()));
  }

  return nearby_process_reference_->GetQuickStartDecoder();
}

void QuickStartConnectivityService::OnNearbyProcessStopped(
    nearby::NearbyProcessManager::NearbyProcessShutdownReason shutdown_reason) {
  // TODO: b/280308935: Handle nearby process shutdown
  nearby_process_reference_ = nullptr;
}

}  // namespace ash::quick_start
