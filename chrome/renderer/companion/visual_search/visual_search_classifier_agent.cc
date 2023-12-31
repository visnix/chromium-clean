// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/companion/visual_search/visual_search_classifier_agent.h"

#include "base/files/file.h"
#include "base/files/memory_mapped_file.h"
#include "base/metrics/histogram_macros_local.h"
#include "base/strings/string_number_conversions.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "chrome/common/companion/visual_search.mojom.h"
#include "chrome/renderer/companion/visual_search/visual_search_classification_and_eligibility.h"
#include "components/optimization_guide/proto/visual_search_model_metadata.pb.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_frame_observer.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_view.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace companion::visual_search {

namespace {

using optimization_guide::proto::EligibilitySpec;
using optimization_guide::proto::FeatureLibrary;
using optimization_guide::proto::OrOfThresholdingRules;
using optimization_guide::proto::ThresholdingRule;

using DOMImageList = base::flat_map<ImageId, SingleImageFeaturesAndBytes>;

// We concurrently on send back up to 2 results of visual classifications.
// The results are not ordered in any way, we simply return the first 4
// items that we get from the classifier.
const int kMaxNumberResults = 2;

EligibilitySpec CreateEligibilitySpec(std::string config_proto) {
  EligibilitySpec eligibility_spec;

  if (!config_proto.empty()) {
    eligibility_spec.ParseFromString(config_proto);
    if (!eligibility_spec.has_additional_cheap_pruning_options()) {
      eligibility_spec.mutable_additional_cheap_pruning_options()
          ->set_z_index_overlap_fraction(0.85);
    }
  } else {
    // This is the default configuration if a config is not provided.
    auto* new_rule = eligibility_spec.add_cheap_pruning_rules()->add_rules();
    new_rule->set_feature_name(FeatureLibrary::IMAGE_VISIBLE_AREA);
    new_rule->set_normalizing_op(FeatureLibrary::BY_VIEWPORT_AREA);
    new_rule->set_thresholding_op(FeatureLibrary::GT);
    new_rule->set_threshold(0.01);
    new_rule = eligibility_spec.add_cheap_pruning_rules()->add_rules();
    new_rule->set_feature_name(FeatureLibrary::IMAGE_FRACTION_VISIBLE);
    new_rule->set_thresholding_op(FeatureLibrary::GT);
    new_rule->set_threshold(0.45);
    new_rule = eligibility_spec.add_cheap_pruning_rules()->add_rules();
    new_rule->set_feature_name(FeatureLibrary::IMAGE_ONPAGE_WIDTH);
    new_rule->set_thresholding_op(FeatureLibrary::GT);
    new_rule->set_threshold(100);
    new_rule = eligibility_spec.add_cheap_pruning_rules()->add_rules();
    new_rule->set_feature_name(FeatureLibrary::IMAGE_ONPAGE_HEIGHT);
    new_rule->set_thresholding_op(FeatureLibrary::GT);
    new_rule->set_threshold(100);
    new_rule = eligibility_spec.add_post_renormalization_rules()->add_rules();
    new_rule->set_feature_name(FeatureLibrary::IMAGE_VISIBLE_AREA);
    new_rule->set_normalizing_op(FeatureLibrary::BY_MAX_VALUE);
    new_rule->set_thresholding_op(FeatureLibrary::GT);
    new_rule->set_threshold(0.5);
    auto* shopping_rule =
        eligibility_spec.add_classifier_score_rules()->add_rules();
    shopping_rule->set_feature_name(FeatureLibrary::SHOPPING_CLASSIFIER_SCORE);
    shopping_rule->set_thresholding_op(FeatureLibrary::GT);
    shopping_rule->set_threshold(0.5);
    auto* sensitivity_rule =
        eligibility_spec.add_classifier_score_rules()->add_rules();
    sensitivity_rule->set_feature_name(FeatureLibrary::SENS_CLASSIFIER_SCORE);
    sensitivity_rule->set_thresholding_op(FeatureLibrary::LT);
    sensitivity_rule->set_threshold(0.5);
    eligibility_spec.mutable_additional_cheap_pruning_options()
        ->set_z_index_overlap_fraction(0.85);
  }

  return eligibility_spec;
}

// Depth-first search for recursively traversing DOM elements and pulling out
// references for images (SkBitmap).
void FindImageElements(blink::WebElement element,
                       std::vector<blink::WebElement>& images) {
  if (element.ImageContents().isNull()) {
    for (blink::WebNode child = element.FirstChild(); !child.IsNull();
         child = child.NextSibling()) {
      if (child.IsElementNode()) {
        FindImageElements(child.To<blink::WebElement>(), images);
      }
    }
  } else {
    if (element.HasAttribute("src")) {
      images.emplace_back(element);
    }
  }
}

// Top-level wrapper call to trigger DOM traversal to find images.
DOMImageList FindImagesOnPage(content::RenderFrame* render_frame) {
  DOMImageList images;
  std::vector<blink::WebElement> image_elements;
  const blink::WebDocument doc = render_frame->GetWebFrame()->GetDocument();
  if (doc.IsNull() || doc.Body().IsNull()) {
    return images;
  }
  FindImageElements(doc.Body(), image_elements);

  int image_counter = 0;
  for (auto& element : image_elements) {
    ImageId id = base::NumberToString(image_counter++);
    images[id] = {
        VisualClassificationAndEligibility::ExtractFeaturesForEligibility(
            id, element),
        element.ImageContents()};
  }

  return images;
}

std::vector<SkBitmap> ClassifyImagesOnBackground(DOMImageList images,
                                                 std::string model_data,
                                                 std::string config_proto,
                                                 gfx::SizeF viewport_size) {
  std::vector<SkBitmap> results;
  const auto classifier = VisualClassificationAndEligibility::Create(
      model_data, CreateEligibilitySpec(config_proto));

  if (classifier == nullptr) {
    LOCAL_HISTOGRAM_BOOLEAN(
        "Companion.VisualSearch.Agent.ClassifierCreationFailure", true);
    return results;
  }

  auto classifier_results =
      classifier->RunClassificationAndEligibility(images, viewport_size);

  int result_counter = 0;
  for (const auto& result : classifier_results) {
    results.emplace_back(images[result].image_contents);
    if (++result_counter >= kMaxNumberResults) {
      break;
    }
  }
  return results;
}

}  // namespace

VisualSearchClassifierAgent::VisualSearchClassifierAgent(
    content::RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame) {
  if (render_frame) {
    render_frame_ = render_frame;
    render_frame->GetAssociatedInterfaceRegistry()
        ->AddInterface<mojom::VisualSuggestionsRequestHandler>(
            base::BindRepeating(
                &VisualSearchClassifierAgent::OnRendererAssociatedRequest,
                base::Unretained(this)));
  }
}

VisualSearchClassifierAgent::~VisualSearchClassifierAgent() = default;

// static
VisualSearchClassifierAgent* VisualSearchClassifierAgent::Create(
    content::RenderFrame* render_frame) {
  return new VisualSearchClassifierAgent(render_frame);
}

void VisualSearchClassifierAgent::StartVisualClassification(
    base::File visual_model,
    const std::string& config_proto,
    mojo::PendingRemote<mojom::VisualSuggestionsResultHandler> result_handler) {
  LOCAL_HISTOGRAM_BOOLEAN("Companion.VisualSearch.Agent.StartClassification",
                          true);
  result_handler_.reset();
  result_handler_.Bind(std::move(result_handler));
  std::vector<SkBitmap> empty_results;

  if (is_classifying_) {
    LOCAL_HISTOGRAM_BOOLEAN(
        "Companion.VisualSearch.Agent.OngoingClassificationFailure",
        is_classifying_);
    OnClassificationDone(empty_results);
    return;
  }

  if (!visual_model.IsValid()) {
    LOCAL_HISTOGRAM_BOOLEAN("Companion.VisualSearch.Agent.InvalidModelFailure",
                            !visual_model.IsValid());
    OnClassificationDone(empty_results);
    return;
  }

  if (!visual_model_.IsValid() &&
      !visual_model_.Initialize(std::move(visual_model))) {
    LOCAL_HISTOGRAM_BOOLEAN("Companion.VisualSearch.Agent.InitModelFailure",
                            true);
    OnClassificationDone(empty_results);
    return;
  }

  is_classifying_ = true;
  std::string model_data =
      std::string(reinterpret_cast<const char*>(visual_model_.data()),
                  visual_model_.length());
  DOMImageList dom_images = FindImagesOnPage(render_frame_);
  LOCAL_HISTOGRAM_COUNTS_100("Companion.VisualSearch.Agent.DomImageCount",
                             dom_images.size());

  blink::WebLocalFrame* frame = render_frame_->GetWebFrame();
  gfx::SizeF viewport_size = frame->View()->VisualViewportSize();
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::BEST_EFFORT},
      base::BindOnce(&ClassifyImagesOnBackground, std::move(dom_images),
                     std::move(model_data), std::move(config_proto),
                     viewport_size),
      base::BindOnce(&VisualSearchClassifierAgent::OnClassificationDone,
                     weak_ptr_factory_.GetWeakPtr()));
}

void VisualSearchClassifierAgent::OnClassificationDone(
    const std::vector<SkBitmap> results) {
  is_classifying_ = false;
  std::vector<mojom::VisualSearchSuggestionPtr> final_results;
  for (const auto& result : results) {
    final_results.emplace_back(mojom::VisualSearchSuggestion::New(result));
  }
  result_handler_->HandleClassification(std::move(final_results));
  LOCAL_HISTOGRAM_COUNTS_100("Companion.VisualSearch.Agent.ClassificationDone",
                             results.size());
}

void VisualSearchClassifierAgent::OnRendererAssociatedRequest(
    mojo::PendingAssociatedReceiver<mojom::VisualSuggestionsRequestHandler>
        receiver) {
  receiver_.reset();
  receiver_.Bind(std::move(receiver));
}

void VisualSearchClassifierAgent::OnDestruct() {
  if (render_frame_) {
    render_frame_->GetAssociatedInterfaceRegistry()->RemoveInterface(
        mojom::VisualSuggestionsRequestHandler::Name_);
  }
  delete this;
}

}  // namespace companion::visual_search
