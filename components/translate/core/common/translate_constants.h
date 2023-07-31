// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_TRANSLATE_CORE_COMMON_TRANSLATE_CONSTANTS_H_
#define COMPONENTS_TRANSLATE_CORE_COMMON_TRANSLATE_CONSTANTS_H_

namespace translate {

// The language code used when the language of a page could not be detected.
// (Matches what the CLD -Compact Language Detection- library reports.)
extern const char* const kUnknownLanguageCode;

// The maximum number of characters allowed for a text selection in Partial
// Translate. Longer selections will be truncated down to the first valid word
// break respecting the threshold.
extern const int kDesktopPartialTranslateTextSelectionMaxCharacters;
// The number of milliseconds to wait before showing the Partial Translate
// bubble, even if no response has been received. In this case, a waiting view
// is shown.
extern const int kDesktopPartialTranslateBubbleShowDelayMs;

// Enum for the Translate.CompactInfobar.Event UMA histogram.
// Note: This enum is used to back an UMA histogram, and should be treated as
// append-only.
// GENERATED_JAVA_ENUM_PACKAGE: org.chromium.chrome.browser.infobar
// GENERATED_JAVA_CLASS_NAME_OVERRIDE: InfobarEvent
enum class InfobarEvent {
  INFOBAR_IMPRESSION = 0,
  INFOBAR_TARGET_TAB_TRANSLATE = 1,
  INFOBAR_DECLINE = 2,
  INFOBAR_OPTIONS = 3,
  INFOBAR_MORE_LANGUAGES = 4,
  INFOBAR_MORE_LANGUAGES_TRANSLATE = 5,
  INFOBAR_PAGE_NOT_IN = 6,
  INFOBAR_ALWAYS_TRANSLATE = 7,
  INFOBAR_NEVER_TRANSLATE = 8,
  INFOBAR_NEVER_TRANSLATE_SITE = 9,
  INFOBAR_SCROLL_HIDE = 10,
  INFOBAR_SCROLL_SHOW = 11,
  INFOBAR_REVERT = 12,
  INFOBAR_SNACKBAR_ALWAYS_TRANSLATE_IMPRESSION = 13,
  INFOBAR_SNACKBAR_NEVER_TRANSLATE_IMPRESSION = 14,
  INFOBAR_SNACKBAR_NEVER_TRANSLATE_SITE_IMPRESSION = 15,
  INFOBAR_SNACKBAR_CANCEL_ALWAYS = 16,
  INFOBAR_SNACKBAR_CANCEL_NEVER_SITE = 17,
  INFOBAR_SNACKBAR_CANCEL_NEVER = 18,
  INFOBAR_ALWAYS_TRANSLATE_UNDO = 19,
  INFOBAR_CLOSE_DEPRECATED = 20,
  INFOBAR_SNACKBAR_AUTO_ALWAYS_IMPRESSION = 21,
  INFOBAR_SNACKBAR_AUTO_NEVER_IMPRESSION = 22,
  INFOBAR_SNACKBAR_CANCEL_AUTO_ALWAYS = 23,
  INFOBAR_SNACKBAR_CANCEL_AUTO_NEVER = 24,
  // 25 was a duplicate code and is now deprecated https://crbug.com/1414604
  INFOBAR_NEVER_TRANSLATE_UNDO = 26,
  INFOBAR_NEVER_TRANSLATE_SITE_UNDO = 27,
  INFOBAR_HISTOGRAM_BOUNDARY = 28,
  kMaxValue = INFOBAR_HISTOGRAM_BOUNDARY,
};

}  // namespace translate

#endif  // COMPONENTS_TRANSLATE_CORE_COMMON_TRANSLATE_CONSTANTS_H_