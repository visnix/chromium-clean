// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_GAME_DASHBOARD_GAME_DASHBOARD_MAIN_MENU_VIEW_H_
#define ASH_GAME_DASHBOARD_GAME_DASHBOARD_MAIN_MENU_VIEW_H_

#include "base/memory/raw_ptr.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"

namespace ash {

class FeatureTile;
class GameDashboardContext;
class PillButton;
class Switch;

// GameDashboardMainMenuView is the expanded menu view attached to the game
// dashboard button.
class GameDashboardMainMenuView : public views::BubbleDialogDelegateView {
 public:
  METADATA_HEADER(GameDashboardMainMenuView);

  explicit GameDashboardMainMenuView(GameDashboardContext* context);

  GameDashboardMainMenuView(views::Widget* main_menu_button_widget,
                            aura::Window* game_window);
  GameDashboardMainMenuView(const GameDashboardMainMenuView&) = delete;
  GameDashboardMainMenuView& operator=(const GameDashboardMainMenuView) =
      delete;
  ~GameDashboardMainMenuView() override;

 private:
  class FeatureDetailsRow;

  // Callbacks for the tiles and buttons in the main menu view.
  // Handles showing and hiding the toolbar.
  void OnToolbarTilePressed();
  // Handles toggling the game recording.
  void OnRecordGameTilePressed();
  // Handles taking a screenshot of the game window when pressed.
  void OnScreenshotTilePressed();

  // Handles functions for Game Controls buttons.
  void OnGameControlsTilePressed();
  void OnGameControlsDetailsPressed();
  void OnGameControlsSetUpButtonPressed();
  void OnGameControlsHintSwitchButtonPressed();

  // Handles when the Screen Size Settings is pressed.
  void OnScreenSizeSettingsButtonPressed();
  // Opens the feedback form with preset information.
  void OnFeedbackButtonPressed();
  // Opens the help center for more info about Game Dashboard.
  void OnHelpButtonPressed();
  // Opens up the Game Dashboard Settings.
  void OnSettingsButtonPressed();

  // Adds a row of shortcut tiles to the main menu view for users to quickly
  // access common functionality.
  void AddShortcutTilesRow();

  // Adds feature details rows, for example, including Game Controls or window
  // size.
  void AddFeatureDetailsRows();

  // Adds Game Controls feature tile in `container` if it is the ARC game window
  // and Game Controls is available.
  void MaybeAddGameControlsTile(views::View* container);

  // Adds menu controls row for Game Controls.
  void MaybeAddGameControlsDetailsRow(views::View* container);

  // Adds a row to access a settings page controlling the screen size if the
  // given game window is an ARC app.
  void MaybeAddScreenSizeSettingsRow(views::View* container);

  // Adds the dashboard cluster (containing feedback, settings, and help
  // buttons) to the Game Controls tile view.
  void AddUtilityClusterRow();

  // Enables Game Controls edit mode.
  void EnableGameControlsEditMode();

  // views::View:
  void VisibilityChanged(views::View* starting_from, bool is_visible) override;

  // Allows this class to access `GameDashboardContext` owned functions/objects.
  const raw_ptr<GameDashboardContext, ExperimentalAsh> context_;

  raw_ptr<FeatureTile> toolbar_tile_ = nullptr;
  raw_ptr<FeatureTile> game_controls_tile_ = nullptr;
  raw_ptr<FeatureDetailsRow> game_controls_details_ = nullptr;
  raw_ptr<PillButton> game_controls_setup_button_ = nullptr;
  raw_ptr<Switch> game_controls_hint_switch_ = nullptr;
};

}  // namespace ash

#endif  // ASH_GAME_DASHBOARD_GAME_DASHBOARD_MAIN_MENU_VIEW_H_
