// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/browser/shell_browser_main_parts.h"

#include "services/device/public/cpp/geolocation/system_geolocation_source_mac.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace content {

device::GeolocationManager* ShellBrowserMainParts::GetGeolocationManager() {
  if (!device::GeolocationManager::GetInstance()) {
    device::GeolocationManager::SetInstance(
        device::SystemGeolocationSourceMac::CreateGeolocationManagerOnMac());
  }
  return device::GeolocationManager::GetInstance();
}

}  // namespace content
