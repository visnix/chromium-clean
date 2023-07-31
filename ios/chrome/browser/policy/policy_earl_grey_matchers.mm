// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ios/chrome/browser/policy/policy_earl_grey_matchers.h"

#import "ios/chrome/test/earl_grey/chrome_earl_grey.h"
#import "ios/chrome/test/earl_grey/chrome_earl_grey_ui.h"
#import "ios/chrome/test/earl_grey/chrome_matchers.h"
#import "ios/chrome/test/earl_grey/chrome_test_case.h"
#import "ios/testing/earl_grey/earl_grey_test.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace policy {

namespace {

void AssertButtonInCollectionWithMatcher(int label_id,
                                         id<GREYMatcher> assertion_matcher) {
  [[EarlGrey
      selectElementWithMatcher:
          grey_allOf(chrome_test_util::ButtonWithAccessibilityLabelId(label_id),
                     grey_ancestor(grey_kindOfClassName(@"UICollectionView")),
                     grey_sufficientlyVisible(), nil)]
      assertWithMatcher:assertion_matcher];
}

}  // namespace

void AssertButtonInCollectionEnabled(int label_id) {
  AssertButtonInCollectionWithMatcher(
      label_id,
      grey_not(grey_accessibilityTrait(UIAccessibilityTraitNotEnabled)));
}

void AssertButtonInCollectionDisabled(int label_id) {
  AssertButtonInCollectionWithMatcher(
      label_id, grey_accessibilityTrait(UIAccessibilityTraitNotEnabled));
}

void AssertOverflowMenuElementEnabled(NSString* accessibilityID) {
  id<GREYMatcher> enabledMatcher =
      [ChromeEarlGrey isNewOverflowMenuEnabled]
          // TODO(crbug.com/1285974): grey_userInteractionEnabled doesn't work
          // for SwiftUI views.
          ? grey_not(grey_accessibilityTrait(UIAccessibilityTraitNotEnabled))
          : grey_userInteractionEnabled();
  [[[EarlGrey selectElementWithMatcher:grey_accessibilityID(accessibilityID)]
         usingSearchAction:grey_scrollInDirection(kGREYDirectionDown,
                                                  /*amount=*/200)
      onElementWithMatcher:chrome_test_util::ToolsMenuView()]
      assertWithMatcher:enabledMatcher];
}

void AssertOverflowMenuElementDisabled(NSString* accessibilityID) {
  [[[EarlGrey selectElementWithMatcher:grey_accessibilityID(accessibilityID)]
         usingSearchAction:grey_scrollInDirection(kGREYDirectionDown,
                                                  /*amount=*/200)
      onElementWithMatcher:chrome_test_util::ToolsMenuView()]
      assertWithMatcher:grey_accessibilityTrait(
                            UIAccessibilityTraitNotEnabled)];
}

}  // namespace policy
