// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/android/persisted_tab_data/persisted_tab_data_config_android.h"

#include "chrome/browser/android/persisted_tab_data/leveldb_persisted_tab_data_storage_android.h"
#include "chrome/browser/android/persisted_tab_data/sensitivity_persisted_tab_data_android.h"

namespace {
const char kSensitivityId[] = "sensitivity";
}  // namespace

PersistedTabDataConfigAndroid::~PersistedTabDataConfigAndroid() = default;

PersistedTabDataConfigAndroid::PersistedTabDataConfigAndroid(
    PersistedTabDataStorageAndroid* persisted_tab_data_storage_android,
    const char* data_id)
    : persisted_tab_data_storage_(persisted_tab_data_storage_android),
      data_id_(data_id) {}

std::unique_ptr<PersistedTabDataConfigAndroid>
PersistedTabDataConfigAndroid::Get(const void* user_data_key,
                                   Profile* profile) {
  if (user_data_key == SensitivityPersistedTabDataAndroid::UserDataKey()) {
    return std::make_unique<PersistedTabDataConfigAndroid>(
        LevelDBPersistedTabDataStorageAndroid::FromProfile(profile),
        kSensitivityId);
  }
  NOTREACHED() << "Unknown UserDataKey";
  return nullptr;
}
