// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_TABS_TAB_SEARCH_BUTTON_H_
#define CHROME_BROWSER_UI_VIEWS_TABS_TAB_SEARCH_BUTTON_H_

#include "chrome/browser/ui/views/tab_search_bubble_host.h"
#include "chrome/browser/ui/views/tabs/tab_strip_control_button.h"
#include "ui/base/metadata/metadata_header_macros.h"

class TabStrip;

// TabSearchButton should leverage the look and feel of the existing
// NewTabButton for sizing and appropriate theming. This class updates the
// NewTabButton with the appropriate icon and will be used to anchor the
// Tab Search bubble.
class TabSearchButton : public TabStripControlButton {
 public:
  METADATA_HEADER(TabSearchButton);
  explicit TabSearchButton(TabStrip* tab_strip);
  TabSearchButton(const TabSearchButton&) = delete;
  TabSearchButton& operator=(const TabSearchButton&) = delete;
  ~TabSearchButton() override;

  TabSearchBubbleHost* tab_search_bubble_host() {
    return tab_search_bubble_host_.get();
  }

  // TabStripControlsButton:
  void NotifyClick(const ui::Event& event) final;
  void UpdateColors() final;

 private:
  int GetCornerRadius();

  std::unique_ptr<TabSearchBubbleHost> tab_search_bubble_host_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_TABS_TAB_SEARCH_BUTTON_H_
