// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/download/download_status_updater.h"

#include <memory>
#include <string>

#include "base/containers/contains.h"
#include "base/notreached.h"
#include "chrome/browser/download/bubble/download_bubble_prefs.h"
#include "chrome/browser/download/download_commands.h"
#include "chrome/browser/download/download_item_model.h"
#include "chrome/browser/download/download_ui_model.h"
#include "chrome/browser/profiles/keep_alive/scoped_profile_keep_alive.h"
#include "chrome/browser/profiles/profile.h"
#include "chromeos/crosapi/mojom/download_controller.mojom.h"
#include "chromeos/crosapi/mojom/download_status_updater.mojom.h"
#include "chromeos/lacros/lacros_service.h"
#include "components/download/public/common/download_item_utils.h"
#include "content/public/browser/download_item_utils.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace {

// Helpers ---------------------------------------------------------------------

crosapi::mojom::DownloadStatusUpdater* GetRemote(
    absl::optional<uint32_t> min_version = absl::nullopt) {
  using DownloadStatusUpdater = crosapi::mojom::DownloadStatusUpdater;
  auto* service = chromeos::LacrosService::Get();
  if (!service || !service->IsAvailable<DownloadStatusUpdater>()) {
    return nullptr;
  }
  // NOTE: Use `remote.version()` rather than `service->GetInterfaceVersion()`
  // as the latter does not respect versions of remotes injected for testing.
  auto& remote = service->GetRemote<DownloadStatusUpdater>();
  return remote.version() >= min_version.value_or(remote.version())
             ? remote.get()
             : nullptr;
}

bool IsCommandEnabled(DownloadItemModel& model,
                      DownloadCommands::Command command) {
  // To support other commands, we may need to update checks below to also
  // inspect `BubbleUIInfo` subpage buttons.
  CHECK(command == DownloadCommands::CANCEL ||
        command == DownloadCommands::PAUSE ||
        command == DownloadCommands::RESUME);

  const bool is_download_bubble_v2_enabled =
      download::IsDownloadBubbleV2Enabled(Profile::FromBrowserContext(
          content::DownloadItemUtils::GetBrowserContext(
              model.GetDownloadItem())));

  // `BubbleUIInfo` contains at most one of either `CANCEL`, `PAUSE`, or
  // `RESUME` when download bubble v2 is disabled, despite the fact that a
  // download may be simultaneously cancellable and pausable/resumable. For
  // this reason, do not use `BubbleUIInfo`-based determination of command
  // enablement when download bubble v2 is disabled.
  if (!is_download_bubble_v2_enabled) {
    DownloadCommands commands(model.GetWeakPtr());
    return model.IsCommandEnabled(&commands, command);
  }

  const DownloadUIModel::BubbleUIInfo info =
      model.GetBubbleUIInfo(/*is_download_bubble_v2_enabled=*/true);

  // A command is enabled if `BubbleUIInfo` contains a quick action for it. This
  // is preferred over non-`BubbleUIInfo`-based determination of command
  // enablement as it takes more signals into account, e.g. if the download has
  // been marked dangerous.
  return base::Contains(info.quick_actions, command,
                        &DownloadUIModel::BubbleUIInfo::QuickAction::command);
}

crosapi::mojom::DownloadStatusPtr ConvertToMojoDownloadStatus(
    download::DownloadItem* download) {
  DownloadItemModel model(download);
  auto status = crosapi::mojom::DownloadStatus::New();
  status->guid = download->GetGuid();
  status->state = download::download_item_utils::ConvertToMojoDownloadState(
      download->GetState());
  status->received_bytes = download->GetReceivedBytes();
  status->total_bytes = download->GetTotalBytes();
  status->target_file_path = download->GetTargetFilePath();
  status->cancellable = IsCommandEnabled(model, DownloadCommands::CANCEL);
  status->pausable = IsCommandEnabled(model, DownloadCommands::PAUSE);
  status->resumable = IsCommandEnabled(model, DownloadCommands::RESUME);
  return status;
}

}  // namespace

// DownloadStatusUpdater::Delegate ---------------------------------------------

// The delegate of the `DownloadStatusUpdater` in Lacros Chrome which serves as
// the client for the `DownloadStatusUpdater` in Ash Chrome.
class DownloadStatusUpdater::Delegate
    : public crosapi::mojom::DownloadStatusUpdaterClient {
 public:
  Delegate() {
    using crosapi::mojom::DownloadStatusUpdater;
    if (auto* remote =
            GetRemote(DownloadStatusUpdater::kBindClientMinVersion)) {
      remote->BindClient(receiver_.BindNewPipeAndPassRemoteWithVersion());
    }
  }

  Delegate(const Delegate&) = delete;
  Delegate& operator=(const Delegate&) = delete;
  ~Delegate() override = default;

 private:
  // crosapi::mojom::DownloadStatusUpdaterClient:
  void Cancel(const std::string& guid, CancelCallback callback) override {
    // TODO(http://b/279794441): Implement.
    NOTIMPLEMENTED();
    std::move(callback).Run(/*handled=*/false);
  }

  void Pause(const std::string& guid, PauseCallback callback) override {
    // TODO(http://b/279794441): Implement.
    NOTIMPLEMENTED();
    std::move(callback).Run(/*handled=*/false);
  }

  void Resume(const std::string& guid, ResumeCallback callback) override {
    // TODO(http://b/279794441): Implement.
    NOTIMPLEMENTED();
    std::move(callback).Run(/*handled=*/false);
  }

  void ShowInBrowser(const std::string& guid,
                     ShowInBrowserCallback callback) override {
    // TODO(http://b/279794441): Implement.
    NOTIMPLEMENTED();
    std::move(callback).Run(/*handled=*/false);
  }

  // The receiver bound to `this` for use by crosapi.
  mojo::Receiver<crosapi::mojom::DownloadStatusUpdaterClient> receiver_{this};
};

// DownloadStatusUpdater -------------------------------------------------------

DownloadStatusUpdater::DownloadStatusUpdater()
    : delegate_(std::make_unique<Delegate>()) {}

DownloadStatusUpdater::~DownloadStatusUpdater() = default;

void DownloadStatusUpdater::UpdateAppIconDownloadProgress(
    download::DownloadItem* download) {
  if (auto* remote = GetRemote()) {
    remote->Update(ConvertToMojoDownloadStatus(download));
  }
}