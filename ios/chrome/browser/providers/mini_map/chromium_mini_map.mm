// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <UIKit/UIKit.h>

#import "ios/public/provider/chrome/browser/mini_map/mini_map_api.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace ios {
namespace provider {

id<MiniMapController> CreateMiniMapController(
    NSString* address,
    MiniMapControllerCompletion completion) {
  // Mini map is not supported in Chromium.
  return nil;
}

}  // namespace provider
}  // namespace ios
