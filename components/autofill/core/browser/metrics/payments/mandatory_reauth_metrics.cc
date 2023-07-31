// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/autofill/core/browser/metrics/payments/mandatory_reauth_metrics.h"

#include "base/metrics/histogram_functions.h"
#include "base/strings/strcat.h"

namespace autofill::autofill_metrics {

namespace {

std::string_view GetSourceForOptInOrOptOutEvent(
    MandatoryReauthOptInOrOutSource source) {
  switch (source) {
    case MandatoryReauthOptInOrOutSource::kSettingsPage:
      return "SettingsPage";
    case MandatoryReauthOptInOrOutSource::kUnknown:
      return "Unknown";
  }
}

}  // namespace

void LogMandatoryReauthOptInBubbleOffer(MandatoryReauthOptInBubbleOffer metric,
                                        bool is_reshow) {
  std::string histogram_name =
      base::StrCat({"Autofill.PaymentMethods.MandatoryReauth.OptInBubbleOffer.",
                    is_reshow ? "Reshow" : "FirstShow"});
  base::UmaHistogramEnumeration(histogram_name, metric);
}

void LogMandatoryReauthOptInBubbleResult(
    MandatoryReauthOptInBubbleResult metric,
    bool is_reshow) {
  std::string histogram_name = base::StrCat(
      {"Autofill.PaymentMethods.MandatoryReauth.OptInBubbleResult.",
       is_reshow ? "Reshow" : "FirstShow"});
  base::UmaHistogramEnumeration(histogram_name, metric);
}

void LogMandatoryReauthOptInConfirmationBubbleMetric(
    MandatoryReauthOptInConfirmationBubbleMetric metric) {
  base::UmaHistogramEnumeration(
      "Autofill.PaymentMethods.MandatoryReauth.OptInConfirmationBubble",
      metric);
}

void LogMandatoryReauthOptInOrOutUpdateEvent(
    MandatoryReauthOptInOrOutSource source,
    bool opt_in,
    MandatoryReauthAuthenticationFlowEvent event) {
  std::string histogram_name = base::StrCat(
      {"Autofill.PaymentMethods.MandatoryReauth.OptChangeEvent.",
       GetSourceForOptInOrOptOutEvent(source), opt_in ? ".OptIn" : ".OptOut"});
  base::UmaHistogramEnumeration(histogram_name, event);
}

}  // namespace autofill::autofill_metrics
