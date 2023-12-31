// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/controls/menu/menu_config.h"

#include <windows.h>  // Must come before other Windows system headers.

#include <Vssym32.h>

#include "base/metrics/histogram_macros.h"
#include "base/win/windows_version.h"
#include "ui/base/ui_base_features.h"
#include "ui/gfx/system_fonts_win.h"

namespace views {

void MenuConfig::Init() {
  font_list =
      gfx::FontList(gfx::win::GetSystemFont(gfx::win::SystemFont::kMenu));

  BOOL show_cues;
  show_mnemonics =
      (SystemParametersInfo(SPI_GETKEYBOARDCUES, 0, &show_cues, 0) &&
       show_cues == TRUE);

  SystemParametersInfo(SPI_GETMENUSHOWDELAY, 0, &show_delay, 0);

  bool is_win11 = base::win::GetVersion() >= base::win::Version::WIN11;
  bool is_refresh = features::IsChromeRefresh2023();
  use_bubble_border = corner_radius > 0 || is_win11;
  UMA_HISTOGRAM_BOOLEAN("Windows.Menu.Win11Style", is_win11 && !is_refresh);
  separator_upper_height = 5;
  separator_lower_height = 7;

  // Under ChromeRefresh2023, the corner radius will not use the win11 style.
  if (use_bubble_border && !is_refresh) {
    corner_radius = 8;
  }
}

}  // namespace views
