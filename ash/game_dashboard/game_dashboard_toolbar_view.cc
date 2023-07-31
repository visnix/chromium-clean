// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/game_dashboard/game_dashboard_toolbar_view.h"

#include <memory>

#include "ash/capture_mode/capture_mode_controller.h"
#include "ash/game_dashboard/game_dashboard_context.h"
#include "ash/game_dashboard/game_dashboard_utils.h"
#include "ash/public/cpp/arc_game_controls_flag.h"
#include "ash/public/cpp/window_properties.h"
#include "ash/resources/vector_icons/vector_icons.h"
#include "ash/strings/grit/ash_strings.h"
#include "ash/style/icon_button.h"
#include "base/types/cxx23_to_underlying.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/chromeos/styles/cros_tokens_color_mappings.h"
#include "ui/views/background.h"

namespace ash {

namespace {

// Horizontal padding for the border around the toolbar.
constexpr int kPaddingWidth = 4;
// Vertical padding for the border around the toolbar.
constexpr int kPaddingHeight = 6;
// Padding between children in the toolbar.
constexpr int kBetweenChildSpacing = 8;

std::unique_ptr<IconButton> CreateIconButton(base::RepeatingClosure callback,
                                             const gfx::VectorIcon* icon,
                                             int view_id,
                                             const std::u16string& text,
                                             bool is_togglable) {
  auto button = std::make_unique<IconButton>(
      std::move(callback), IconButton::Type::kSmallFloating, icon, text,
      /*is_togglable=*/is_togglable, /*has_border=*/true);
  button->SetID(view_id);
  return button;
}

}  // namespace

GameDashboardToolbarView::GameDashboardToolbarView(
    GameDashboardContext* context)
    : context_(context) {
  DCHECK(context_);
  DCHECK(context_->game_window());

  SetOrientation(views::BoxLayout::Orientation::kVertical);
  SetInsideBorderInsets(gfx::Insets::VH(kPaddingHeight, kPaddingWidth));
  SetBetweenChildSpacing(kBetweenChildSpacing);
  SetCrossAxisAlignment(views::BoxLayout::CrossAxisAlignment::kCenter);
  SetBackground(
      views::CreateThemedSolidBackground(cros_tokens::kCrosSysBaseElevated));

  AddShortcutTiles();
}

GameDashboardToolbarView::~GameDashboardToolbarView() {
  context_->game_window()->RemoveObserver(this);
}

void GameDashboardToolbarView::OnGamepadButtonPressed() {
  is_expanded_ = !is_expanded_;
  for (View* child : children()) {
    if (child != gamepad_button_) {
      child->SetVisible(is_expanded_);
    }
  }
  context_->MaybeUpdateToolbarWidgetBounds();
}

void GameDashboardToolbarView::OnGameControlsButtonPressed() {
  auto* game_window = context_->game_window();
  game_window->SetProperty(
      kArcGameControlsFlagsKey,
      game_dashboard_utils::UpdateFlag(
          game_window->GetProperty(kArcGameControlsFlagsKey),
          static_cast<ArcGameControlsFlag>(ArcGameControlsFlag::kEnabled |
                                           ArcGameControlsFlag::kHint),
          /*enable_flag=*/!game_controls_button_->toggled()));
}

void GameDashboardToolbarView::OnRecordButtonPressed() {
  // TODO(b/273641250): Add screen record support.
}

void GameDashboardToolbarView::OnScreenshotButtonPressed() {
  CaptureModeController::Get()->CaptureScreenshotOfGivenWindow(
      context_->game_window());
}

void GameDashboardToolbarView::AddShortcutTiles() {
  // The gamepad button should always be the first icon added to the toolbar.
  gamepad_button_ = AddChildView(CreateIconButton(
      base::BindRepeating(&GameDashboardToolbarView::OnGamepadButtonPressed,
                          base::Unretained(this)),
      &kGdToolbarIcon, base::to_underlying(ToolbarViewId::kGamepadButton),
      l10n_util::GetStringUTF16(
          IDS_ASH_GAME_DASHBOARD_TOOLBAR_TILE_BUTTON_TITLE),
      /*is_togglable=*/false));

  MayAddGameControlsTile();

  // TODO(b/273641250): Filter out record game button based on device info.
  AddChildView(CreateIconButton(
      base::BindRepeating(&GameDashboardToolbarView::OnRecordButtonPressed,
                          base::Unretained(this)),
      &kGdRecordGameIcon,
      base::to_underlying(ToolbarViewId::kScreenRecordButton),
      l10n_util::GetStringUTF16(
          IDS_ASH_GAME_DASHBOARD_RECORD_GAME_TILE_BUTTON_TITLE),
      /*is_togglable=*/false));

  AddChildView(CreateIconButton(
      base::BindRepeating(&GameDashboardToolbarView::OnScreenshotButtonPressed,
                          base::Unretained(this)),
      &kGdScreenshotIcon, base::to_underlying(ToolbarViewId::kScreenshotButton),
      l10n_util::GetStringUTF16(
          IDS_ASH_GAME_DASHBOARD_SCREENSHOT_TILE_BUTTON_TITLE),
      /*is_togglable=*/false));
}

void GameDashboardToolbarView::MayAddGameControlsTile() {
  auto flags =
      game_dashboard_utils::GetGameControlsFlag(context_->game_window());
  if (!flags) {
    return;
  }

  // Add observer to check window property change on `kArcGameControlsFlagsKey`.
  context_->game_window()->AddObserver(this);

  game_controls_button_ = AddChildView(CreateIconButton(
      base::BindRepeating(
          &GameDashboardToolbarView::OnGameControlsButtonPressed,
          base::Unretained(this)),
      /*icon=*/&kGdGameControlsIcon,
      base::to_underlying(ToolbarViewId::kGameControlsButton),
      l10n_util::GetStringUTF16(
          IDS_ASH_GAME_DASHBOARD_CONTROLS_TILE_BUTTON_TITLE),
      /*is_togglable=*/true));
  game_controls_button_->SetEnabled(
      !game_dashboard_utils::IsFlagSet(*flags, ArcGameControlsFlag::kEmpty));
  if (game_controls_button_->GetEnabled()) {
    game_controls_button_->SetToggled(
        game_dashboard_utils::IsFlagSet(*flags, ArcGameControlsFlag::kEnabled));
  }
}

void GameDashboardToolbarView::OnWindowPropertyChanged(aura::Window* window,
                                                       const void* key,
                                                       intptr_t old) {
  // Once the main menu changes Game Controls states, this view should also
  // reflect the same states.
  if (key != ash::kArcGameControlsFlagsKey) {
    return;
  }
  CHECK_EQ(window, context_->game_window());

  ArcGameControlsFlag new_flags = window->GetProperty(kArcGameControlsFlagsKey);
  ArcGameControlsFlag old_flags = static_cast<ash::ArcGameControlsFlag>(old);

  if (game_dashboard_utils::IsFlagChanged(new_flags, old_flags,
                                          ArcGameControlsFlag::kEmpty)) {
    game_controls_button_->SetEnabled(!game_dashboard_utils::IsFlagSet(
        new_flags, ArcGameControlsFlag::kEmpty));
  }

  if (game_dashboard_utils::IsFlagChanged(new_flags, old_flags,
                                          ArcGameControlsFlag::kEnabled)) {
    game_controls_button_->SetToggled(game_dashboard_utils::IsFlagSet(
        new_flags, ArcGameControlsFlag::kEnabled));
  }
}

BEGIN_METADATA(GameDashboardToolbarView, views::BoxLayoutView)
END_METADATA

}  // namespace ash
