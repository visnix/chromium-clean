// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_ANDROID_PERSISTED_TAB_DATA_PERSISTED_TAB_DATA_STORAGE_ANDROID_H_
#define CHROME_BROWSER_ANDROID_PERSISTED_TAB_DATA_PERSISTED_TAB_DATA_STORAGE_ANDROID_H_

#include <vector>

#include "base/functional/callback_forward.h"

// Storage interface used for PersistedTabDataAndroid
class PersistedTabDataStorageAndroid {
 public:
  PersistedTabDataStorageAndroid() = default;
  virtual ~PersistedTabDataStorageAndroid() = default;
  using RestoreCallback =
      base::OnceCallback<void(const std::vector<uint8_t>& data)>;

  // Save data for a Tab ID, Data ID pair
  virtual void Save(int tab_id,
                    const char* data_id,
                    const std::vector<uint8_t>& data);

  // Restore data for a Tab ID, Data ID pair
  virtual void Restore(int tab_id,
                       const char* data_id,
                       RestoreCallback restore_callback);

  // Remove data for a Tab ID, Data ID pair
  virtual void Remove(int tab_id, const char* data_id);
};

#endif  // CHROME_BROWSER_ANDROID_PERSISTED_TAB_DATA_PERSISTED_TAB_DATA_STORAGE_ANDROID_H_
