// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/ui/toolbar/fullscreen/toolbar_ui.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

@implementation ToolbarUIState
@synthesize collapsedTopToolbarHeight = _collapsedTopToolbarHeight;
@synthesize expandedTopToolbarHeight = _expandedTopToolbarHeight;
@synthesize expandedBottomToolbarHeight = _expandedBottomToolbarHeight;
@synthesize collapsedBottomToolbarHeight = _collapsedBottomToolbarHeight;
@end
