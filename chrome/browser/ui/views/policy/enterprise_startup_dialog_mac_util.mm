// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "chrome/browser/ui/views/policy/enterprise_startup_dialog_mac_util.h"

#include <Cocoa/Cocoa.h>

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace policy {

void StartModal(gfx::NativeWindow dialog) {
  [NSApp runModalForWindow:dialog.GetNativeNSWindow()];
}
void StopModal() {
  [NSApp stopModal];
}

}  // namespace policy
