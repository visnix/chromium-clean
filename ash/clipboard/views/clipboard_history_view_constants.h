// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_CLIPBOARD_VIEWS_CLIPBOARD_HISTORY_VIEW_CONSTANTS_H_
#define ASH_CLIPBOARD_VIEWS_CLIPBOARD_HISTORY_VIEW_CONSTANTS_H_

#include "ui/gfx/geometry/insets.h"

namespace ash::ClipboardHistoryViews {

// The insets within the contents view.
constexpr int kContentsVerticalInset = 8;
constexpr auto kContentsInsets = gfx::Insets::VH(kContentsVerticalInset, 16);

// The padding vertically separating the Ctrl+V label from the contents view.
constexpr int kCtrlVLabelPadding = 6;

// The size of the `DeleteButton`.
constexpr int kDeleteButtonSizeDip = 16;

// The maximum number of lines allotted to a text item's label when the
// clipboard history refresh is enabled.
constexpr size_t kTextItemMaxLines = 2u;

// The margins of the `DeleteButton` instance showing on a
// `ClipboardHistoryTextItemView`.
constexpr auto kTextItemDeleteButtonMargins = gfx::Insets::TLBR(0, 8, 0, 4);

// The margins of the `DeleteButton` instance showing on
// `ClipboardHistoryBitmapItemView`.
constexpr auto kBitmapItemDeleteButtonMargins = gfx::Insets::TLBR(4, 0, 0, 4);

// The width and height of the placeholder icon for an unrendered HTML preview.
constexpr int kBitmapItemPlaceholderIconSize = 32;

// The preferred height of `ClipboardHistoryLabel`.
constexpr int kLabelPreferredHeight = 20;

// The preferred height of the image view showing on
// `ClipboardHistoryBitmapItemView`.
constexpr int kImageViewPreferredHeight = 72;

// The radius of the image view's rounded corners when offset by a background.
constexpr float kImageBackgroundCornerRadius = 12.f;

// The radius of the image view's rounded corners when surrounded by a border.
constexpr float kImageBorderCornerRadius = 4.f;

// The height of the region cut out from a contents view when the refresh is
// enabled and the menu item's delete button is showing.
constexpr float kCornerCutoutHeight = 38.f;

// The preferred size for an item's icon.
constexpr gfx::Size kIconSize(20, 20);

// The margins for an item's icon.
constexpr auto kIconMargins = gfx::Insets::TLBR(0, 0, 0, 12);

// The thickness of the image border.
constexpr int kImageBorderThickness = 1;

}  // namespace ash::ClipboardHistoryViews

#endif  // ASH_CLIPBOARD_VIEWS_CLIPBOARD_HISTORY_VIEW_CONSTANTS_H_
