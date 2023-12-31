// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/local_discovery/service_discovery_client_mac_factory.h"

#include "chrome/browser/local_discovery/service_discovery_client_mac.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace local_discovery {

// static
scoped_refptr<ServiceDiscoverySharedClient>
ServiceDiscoveryClientMacFactory::CreateInstance() {
  return new ServiceDiscoveryClientMac();
}

}  // namespace local_discovery
