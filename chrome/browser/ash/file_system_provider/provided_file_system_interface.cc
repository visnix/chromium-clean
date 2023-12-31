// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ash/file_system_provider/provided_file_system_interface.h"

namespace ash {
namespace file_system_provider {

EntryMetadata::EntryMetadata() {}

EntryMetadata::~EntryMetadata() {
}

OpenedFile::OpenedFile(const base::FilePath& file_path, OpenFileMode mode)
    : file_path(file_path), mode(mode) {}

OpenedFile::OpenedFile() : mode(OPEN_FILE_MODE_READ) {
}

OpenedFile::~OpenedFile() {
}

ScopedUserInteraction::ScopedUserInteraction() = default;
ScopedUserInteraction::~ScopedUserInteraction() = default;
ScopedUserInteraction::ScopedUserInteraction(ScopedUserInteraction&&) = default;
ScopedUserInteraction& ScopedUserInteraction::operator=(
    ScopedUserInteraction&&) = default;

}  // namespace file_system_provider
}  // namespace ash
