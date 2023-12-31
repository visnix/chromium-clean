// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_ASH_CLOUD_UPLOAD_CLOUD_UPLOAD_UTIL_H_
#define CHROME_BROWSER_UI_WEBUI_ASH_CLOUD_UPLOAD_CLOUD_UPLOAD_UTIL_H_

#include <string>
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/functional/callback.h"
#include "chrome/browser/ash/file_manager/io_task.h"
#include "chrome/browser/ash/file_system_provider/provided_file_system_interface.h"
#include "chrome/browser/platform_util.h"
#include "storage/browser/file_system/file_system_context.h"
#include "storage/browser/file_system/file_system_url.h"

class Profile;

namespace ash::cloud_upload {

struct ODFSMetadata {
  bool reauthentication_required = false;
  std::string user_email;
};

typedef base::OnceCallback<void(
    base::expected<ODFSMetadata, base::File::Error> metadata_or_error)>
    GetODFSMetadataCallback;

// Type of the source location from which a given file is being uploaded.
enum class SourceType {
  LOCAL = 0,
  READ_ONLY = 1,
  CLOUD = 2,
  kMaxValue = CLOUD,
};

// Type of upload of a file to the Cloud.
enum class UploadType {
  kCopy = 0,
  kMove = 1,
  kMaxValue = kMove,
};

// The result of the "Upload to cloud" workflow for Office files.
//
// These values are persisted to logs. Entries should not be renumbered and
// numeric values should never be reused.
enum class OfficeFilesUploadResult {
  kSuccess = 0,
  kOtherError = 1,
  kFileSystemNotFound = 2,
  kMoveOperationCancelled = 3,
  kMoveOperationError = 4,
  kMoveOperationNeedPassword = 5,
  kCopyOperationCancelled = 6,
  kCopyOperationError = 7,
  kCopyOperationNeedPassword = 8,
  kPinningFailedDiskFull = 9,
  kCloudAuthError = 10,
  kCloudMetadataError = 11,
  kCloudQuotaFull = 12,
  kCloudError = 13,
  kNoConnection = 14,
  kMaxValue = kNoConnection,
};

// Query actions for this path to get ODFS Metadata.
const char kODFSMetadataQueryPath[] = "/";

// Custom action ids passed from ODFS.
const char kOneDriveUrlActionId[] = "HIDDEN_ONEDRIVE_URL";
const char kUserEmailActionId[] = "HIDDEN_ONEDRIVE_USER_EMAIL";
const char kReauthenticationRequiredId[] =
    "HIDDEN_ONEDRIVE_REAUTHENTICATION_REQUIRED";

// Get generic error message for uploading office files.
std::string GetGenericErrorMessage();
// Get Microsoft authentication error message for uploading office files.
std::string GetReauthenticationRequiredMessage();
// Get access denied error message.
std::string GetGenericOneDriveAccessErrorMessage();

// Converts an absolute FilePath into a filesystem URL.
storage::FileSystemURL FilePathToFileSystemURL(
    Profile* profile,
    scoped_refptr<storage::FileSystemContext> file_system_context,
    base::FilePath file_path);

// Creates a directory from a filesystem URL. The callback is called without
// error if the directory already exists.
void CreateDirectoryOnIOThread(
    scoped_refptr<storage::FileSystemContext> file_system_context,
    storage::FileSystemURL destination_folder_url,
    base::OnceCallback<void(base::File::Error)> complete_callback);

// Returns the type of the source location from which the file is getting
// uploaded (see SourceType values).
SourceType GetSourceType(Profile* profile,
                         const storage::FileSystemURL& source_path);

// Returns the upload type (move or copy) for the upload flow based on the
// source path of the file to upload.
UploadType GetUploadType(Profile* profile,
                         const storage::FileSystemURL& source_path);

// Get information of the currently provided ODFS. Expect there to be exactly
// one ODFS.
absl::optional<file_system_provider::ProvidedFileSystemInfo> GetODFSInfo(
    Profile* profile);

// Get currently provided ODFS.
absl::optional<file_system_provider::ProvidedFileSystemInterface*> GetODFS(
    Profile* profile);

// Get ODFS metadata as actions by doing a special GetActions request (for the
// root directory) and return the actions to |OnODFSMetadataActions| which will
// be converted to |ODFSMetadata| and passed to |callback|.
void GetODFSMetadata(
    file_system_provider::ProvidedFileSystemInterface* file_system,
    GetODFSMetadataCallback callback);

}  // namespace ash::cloud_upload

#endif  // CHROME_BROWSER_UI_WEBUI_ASH_CLOUD_UPLOAD_CLOUD_UPLOAD_UTIL_H_
