// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "headless/lib/headless_content_main_delegate.h"

#include "headless/lib/browser/headless_shell_application_mac.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace headless {

void HeadlessContentMainDelegate::PlatformPreBrowserMain() {
  // Force the NSApplication subclass to be used.
  [HeadlessShellCrApplication sharedApplication];

  // If there was an invocation to NSApp prior to this method, then the NSApp
  // will not be a HeadlessShellCrApplication, but will instead be an
  // NSApplication. This is undesirable and we must enforce that this doesn't
  // happen.
  CHECK([NSApp isKindOfClass:[HeadlessShellCrApplication class]]);
}

}  // namespace headless
