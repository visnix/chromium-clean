// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/services/system_signals/mac/mac_system_signals_service.h"

#include <utility>

#include "components/device_signals/core/common/common_types.h"
#include "components/device_signals/core/system_signals/executable_metadata_service.h"
#include "components/device_signals/core/system_signals/file_system_service.h"
#include "components/device_signals/core/system_signals/mac/mac_platform_delegate.h"
#include "components/device_signals/core/system_signals/platform_delegate.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace system_signals {

MacSystemSignalsService::MacSystemSignalsService(
    mojo::PendingReceiver<device_signals::mojom::SystemSignalsService> receiver)
    : MacSystemSignalsService(
          std::move(receiver),
          device_signals::FileSystemService::Create(
              std::make_unique<device_signals::MacPlatformDelegate>(),
              device_signals::ExecutableMetadataService::Create(
                  std::make_unique<device_signals::MacPlatformDelegate>()))) {}

MacSystemSignalsService::MacSystemSignalsService(
    mojo::PendingReceiver<device_signals::mojom::SystemSignalsService> receiver,
    std::unique_ptr<device_signals::FileSystemService> file_system_service)
    : BaseSystemSignalsService(std::move(receiver),
                               std::move(file_system_service)) {}

MacSystemSignalsService::~MacSystemSignalsService() = default;

}  // namespace system_signals
