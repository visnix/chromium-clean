// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/allocator/partition_allocator/partition_alloc_base/system/sys_info.h"

#import <Foundation/Foundation.h>

#include "base/allocator/partition_allocator/partition_alloc_base/numerics/safe_conversions.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace partition_alloc::internal::base {

// static
void SysInfo::OperatingSystemVersionNumbers(int32_t* major_version,
                                            int32_t* minor_version,
                                            int32_t* bugfix_version) {
  NSOperatingSystemVersion version =
      [[NSProcessInfo processInfo] operatingSystemVersion];
  *major_version = saturated_cast<int32_t>(version.majorVersion);
  *minor_version = saturated_cast<int32_t>(version.minorVersion);
  *bugfix_version = saturated_cast<int32_t>(version.patchVersion);
}

}  // namespace partition_alloc::internal::base
