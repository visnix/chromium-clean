// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/location_bar/cookie_controls/cookie_controls_content_view.h"

#include "chrome/app/vector_icons/vector_icons.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/controls/rich_controls_container_view.h"
#include "chrome/browser/ui/views/controls/rich_hover_button.h"
#include "chrome/grit/generated_resources.h"
#include "components/vector_icons/vector_icons.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/ui_base_features.h"
#include "ui/views/controls/button/toggle_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/separator.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/view_class_properties.h"

namespace {

constexpr int kDefaultIconSize = 16;
constexpr int kDefaultIconSizeChromeRefresh = 20;

int GetDefaultIconSize() {
  return features::IsChromeRefresh2023() ? kDefaultIconSizeChromeRefresh
                                         : kDefaultIconSize;
}

std::unique_ptr<views::View> CreateSeparator() {
  const int separator_padding = ChromeLayoutProvider::Get()->GetDistanceMetric(
      DISTANCE_HORIZONTAL_SEPARATOR_PADDING_PAGE_INFO_VIEW);
  int separator_spacing = ChromeLayoutProvider::Get()->GetDistanceMetric(
      DISTANCE_CONTENT_LIST_VERTICAL_MULTI);
  if (!features::IsChromeRefresh2023()) {
    // Distance for multi content list is used, but split in half, since there
    // is a separator in the middle of it. For ChromeRefresh2023, the separator
    // spacing is larger hence no need to split in half.
    separator_spacing /= 2;
  }
  auto separator = std::make_unique<views::Separator>();
  separator->SetProperty(views::kMarginsKey,
                         gfx::Insets::VH(separator_spacing, separator_padding));
  return separator;
}

}  // namespace

DEFINE_CLASS_ELEMENT_IDENTIFIER_VALUE(CookieControlsContentView, kTitle);
DEFINE_CLASS_ELEMENT_IDENTIFIER_VALUE(CookieControlsContentView, kDescription);
DEFINE_CLASS_ELEMENT_IDENTIFIER_VALUE(CookieControlsContentView, kToggleButton);
DEFINE_CLASS_ELEMENT_IDENTIFIER_VALUE(CookieControlsContentView,
                                      kFeedbackButton);

CookieControlsContentView::CookieControlsContentView() {
  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));

  AddChildView(CreateSeparator());
  AddContentLabels();
  AddToggleRow();
  AddFeedbackSection();
}

void CookieControlsContentView::AddContentLabels() {
  auto* provider = ChromeLayoutProvider::Get();
  const int vertical_margin =
      provider->GetDistanceMetric(DISTANCE_CONTENT_LIST_VERTICAL_MULTI);
  const int side_margin =
      provider->GetInsetsMetric(views::INSETS_DIALOG).left();

  auto* label_wrapper = AddChildView(std::make_unique<views::View>());
  label_wrapper->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));
  label_wrapper->SetProperty(views::kMarginsKey,
                             gfx::Insets::VH(vertical_margin, side_margin));
  title_ = label_wrapper->AddChildView(std::make_unique<views::Label>());
  title_->SetTextContext(views::style::CONTEXT_DIALOG_BODY_TEXT);
  title_->SetTextStyle(views::style::STYLE_PRIMARY);
  title_->SetHorizontalAlignment(gfx::HorizontalAlignment::ALIGN_LEFT);
  title_->SetProperty(views::kElementIdentifierKey, kTitle);

  description_ = label_wrapper->AddChildView(std::make_unique<views::Label>());
  description_->SetTextContext(views::style::CONTEXT_LABEL);
  description_->SetTextStyle(views::style::STYLE_SECONDARY);
  description_->SetHorizontalAlignment(gfx::HorizontalAlignment::ALIGN_LEFT);
  description_->SetMultiLine(true);
  description_->SetProperty(views::kElementIdentifierKey, kDescription);
}

void CookieControlsContentView::SetToggleIsOn(bool is_on) {
  toggle_button_->AnimateIsOn(is_on);
}

void CookieControlsContentView::SetToggleIcon(const gfx::VectorIcon& icon) {
  toggle_row_->SetIcon(ui::ImageModel::FromVectorIcon(icon, ui::kColorIcon,
                                                      GetDefaultIconSize()));
}

void CookieControlsContentView::SetToggleVisible(bool visible) {
  toggle_button_->SetVisible(visible);
  PreferredSizeChanged();
}

void CookieControlsContentView::SetEnforcedIcon(const gfx::VectorIcon& icon,
                                                const std::u16string& tooltip) {
  enforced_icon_->SetImage(ui::ImageModel::FromVectorIcon(
      icon, ui::kColorIcon, GetDefaultIconSize()));
  enforced_icon_->SetTooltipText(tooltip);
}

void CookieControlsContentView::SetEnforcedIconVisible(bool visible) {
  enforced_icon_->SetVisible(visible);
}

void CookieControlsContentView::SetFeedbackSectionVisibility(bool visible) {
  feedback_section_->SetVisible(visible);
  PreferredSizeChanged();
}

void CookieControlsContentView::AddToggleRow() {
  toggle_row_ = AddChildView(std::make_unique<RichControlsContainerView>());
  toggle_row_->SetTitle(l10n_util::GetStringUTF16(
      IDS_COOKIE_CONTROLS_BUBBLE_THIRD_PARTY_COOKIES_LABEL));

  // TODO (crbug.com/1446230): Use plural string and update label based on
  // actual blocked sites.
  const std::u16string secondary_label = u"17 sites blocked";
  toggle_row_->AddSecondaryLabel(secondary_label);

  enforced_icon_ =
      toggle_row_->AddControl(std::make_unique<views::ImageView>());

  toggle_button_ = toggle_row_->AddControl(
      std::make_unique<views::ToggleButton>(base::BindRepeating(
          &CookieControlsContentView::NotifyToggleButtonPressedCallback,
          base::Unretained(this))));
  toggle_button_->SetPreferredSize(
      gfx::Size(toggle_button_->GetPreferredSize().width(),
                toggle_row_->GetFirstLineHeight()));

  const std::u16string accessible_name = base::JoinString(
      {
          l10n_util::GetStringUTF16(
              IDS_COOKIE_CONTROLS_BUBBLE_THIRD_PARTY_COOKIES_LABEL),
          secondary_label,
      },
      u"\n");
  toggle_button_->SetAccessibleName(accessible_name);
  toggle_button_->SetVisible(true);
  toggle_button_->SetProperty(views::kElementIdentifierKey, kToggleButton);
}

void CookieControlsContentView::AddFeedbackSection() {
  feedback_section_ = AddChildView(std::make_unique<views::View>());
  feedback_section_->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));

  const ui::ImageModel feedback_icon = ui::ImageModel::FromVectorIcon(
      kSubmitFeedbackIcon, ui::kColorMenuIcon, kDefaultIconSize);
  const ui::ImageModel launch_icon = ui::ImageModel::FromVectorIcon(
      vector_icons::kLaunchIcon, ui::kColorMenuIcon, kDefaultIconSize);

  feedback_section_->AddChildView(CreateSeparator());

  auto* feedback_button =
      feedback_section_->AddChildView(std::make_unique<RichHoverButton>(
          base::BindRepeating(
              &CookieControlsContentView::NotifyFeedbackButtonPressedCallback,
              base::Unretained(this)),
          feedback_icon,
          l10n_util::GetStringUTF16(
              IDS_COOKIE_CONTROLS_BUBBLE_SEND_FEEDBACK_BUTTON_TITLE),
          std::u16string(),
          l10n_util::GetStringUTF16(
              IDS_COOKIE_CONTROLS_BUBBLE_SEND_FEEDBACK_BUTTON_TITLE),
          l10n_util::GetStringUTF16(
              IDS_COOKIE_CONTROLS_BUBBLE_SEND_FEEDBACK_BUTTON_DESCRIPTION),
          launch_icon));

  feedback_button->SetProperty(views::kElementIdentifierKey, kFeedbackButton);
  feedback_button->SetAccessibleName(l10n_util::GetStringUTF16(
      IDS_COOKIE_CONTROLS_BUBBLE_SEND_FEEDBACK_BUTTON_TITLE));
}

void CookieControlsContentView::UpdateContentLabels(
    const std::u16string& title,
    const std::u16string& description) {
  title_->SetText(title);
  description_->SetText(description);
  PreferredSizeChanged();
}

void CookieControlsContentView::SetContentLabelsVisible(bool visible) {
  title_->SetVisible(visible);
  description_->SetVisible(visible);
  PreferredSizeChanged();
}

CookieControlsContentView::~CookieControlsContentView() = default;

base::CallbackListSubscription
CookieControlsContentView::RegisterToggleButtonPressedCallback(
    base::RepeatingCallback<void(bool)> callback) {
  return toggle_button_callback_list_.Add(std::move(callback));
}

base::CallbackListSubscription
CookieControlsContentView::RegisterFeedbackButtonPressedCallback(
    base::RepeatingClosureList::CallbackType callback) {
  return feedback_button_callback_list_.Add(std::move(callback));
}

void CookieControlsContentView::NotifyToggleButtonPressedCallback() {
  toggle_button_callback_list_.Notify(toggle_button_->GetIsOn());
}

void CookieControlsContentView::NotifyFeedbackButtonPressedCallback() {
  feedback_button_callback_list_.Notify();
}
