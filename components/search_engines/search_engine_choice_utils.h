// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_SEARCH_ENGINES_SEARCH_ENGINE_CHOICE_UTILS_H_
#define COMPONENTS_SEARCH_ENGINES_SEARCH_ENGINE_CHOICE_UTILS_H_

namespace policy {
class PolicyService;
}

namespace search_engines {

// Profile properties that need to be passed to
// `ShouldShowSearchEngineChoiceScreen`. This is due to the fact that
// the 'Profile' class is different between platforms.
struct ProfileProperties {
  bool is_regular_profile = false;
};

// Returns whether the search engine choice screen can be displayed or not based
// on device policies and profile properties.
bool ShouldShowChoiceScreen(const policy::PolicyService& policy_service,
                            const ProfileProperties& profile_properties);

}  // namespace search_engines

#endif  // COMPONENTS_SEARCH_ENGINES_SEARCH_ENGINE_CHOICE_UTILS_H_
