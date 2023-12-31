// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/emoji/emoji_panel_helper.h"

#import <Cocoa/Cocoa.h>

#include "base/feature_list.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace ui {

bool IsEmojiPanelSupported() {
  return true;
}

void ShowEmojiPanel() {
  [NSApp orderFrontCharacterPalette:nil];
}

}  // namespace ui
