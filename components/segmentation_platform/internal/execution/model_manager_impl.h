// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_SEGMENTATION_PLATFORM_INTERNAL_EXECUTION_MODEL_MANAGER_IMPL_H_
#define COMPONENTS_SEGMENTATION_PLATFORM_INTERNAL_EXECUTION_MODEL_MANAGER_IMPL_H_

#include <map>
#include <memory>
#include <vector>

#include "base/containers/flat_set.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "components/segmentation_platform/internal/database/segment_info_database.h"
#include "components/segmentation_platform/internal/execution/default_model_manager.h"
#include "components/segmentation_platform/internal/execution/model_manager.h"
#include "components/segmentation_platform/public/proto/segmentation_platform.pb.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace base {
class Clock;
}  // namespace base

namespace segmentation_platform {
class ModelProvider;
class ModelProviderFactory;

namespace proto {
class SegmentInfo;
}  // namespace proto

// ModelManager that owns and manages non-default (OptimizationGuide
// based) ModelProvider(s) and uses SegmentInfoDatabase (metadata) for storing
// the latest model from ModelProvider. The vector of OptimizationTargets need
// to be passed in at construction time so the SegmentationModelHandler
// instances can be created early.
class ModelManagerImpl : public ModelManager {
 public:
  ModelManagerImpl(
      const base::flat_set<SegmentId>& segment_ids,
      ModelProviderFactory* model_provider_factory,
      base::Clock* clock,
      SegmentInfoDatabase* segment_database,
      DefaultModelManager* default_model_manager,
      const SegmentationModelUpdatedCallback& model_updated_callback);
  ~ModelManagerImpl() override;

  // Disallow copy/assign.
  ModelManagerImpl(const ModelManagerImpl&) = delete;
  ModelManagerImpl& operator=(const ModelManagerImpl&) = delete;

  // ModelManager override:
  ModelProvider* GetModelProvider(proto::SegmentId segment_id,
                                  proto::ModelSource model_source) override;

 private:
  friend class SegmentationPlatformServiceImplTest;
  friend class TestServicesForPlatform;

  // Callback for whenever a SegmentationModelHandler is informed that the
  // underlying ML model file has been updated. If there is an available
  // model, this will be called at least once per session.
  void OnSegmentationModelUpdated(proto::ModelSource model_source,
                                  proto::SegmentId segment_id,
                                  proto::SegmentationModelMetadata metadata,
                                  int64_t model_version);

  // Callback after fetching the current SegmentInfo from the
  // SegmentInfoDatabase. This is part of the flow for informing the
  // SegmentationModelUpdatedCallback about a changed model.
  // Merges the PredictionResult from the previously stored SegmentInfo with
  // the newly updated one, and stores the new version in the DB.
  void OnSegmentInfoFetchedForModelUpdate(
      proto::SegmentId segment_id,
      proto::ModelSource model_source,
      proto::SegmentationModelMetadata metadata,
      int64_t model_version,
      absl::optional<proto::SegmentInfo> segment_info);

  // Callback after storing the updated version of the SegmentInfo.
  // Responsible for invoking the SegmentationModelUpdatedCallback.
  void OnUpdatedSegmentInfoStored(proto::SegmentInfo segment_info,
                                  bool success);

  // All the relevant handlers for each of the segments.
  std::map<std::pair<SegmentId, proto::ModelSource>,
           std::unique_ptr<ModelProvider>>
      model_providers_;

  // Used to access the current time.
  raw_ptr<base::Clock> clock_;

  // Database for segment information and metadata.
  raw_ptr<SegmentInfoDatabase> segment_database_;

  // Class to get segment info from default models.
  const raw_ptr<DefaultModelManager> default_model_manager_;

  // Invoked whenever there is an update to any of the relevant ML models.
  SegmentationModelUpdatedCallback model_updated_callback_;

  base::WeakPtrFactory<ModelManagerImpl> weak_ptr_factory_{this};
};

}  // namespace segmentation_platform

#endif  // COMPONENTS_SEGMENTATION_PLATFORM_INTERNAL_EXECUTION_MODEL_MANAGER_IMPL_H_
