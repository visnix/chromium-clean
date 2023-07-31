// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_AUTOFILL_CORE_BROWSER_METRICS_PAYMENTS_MANDATORY_REAUTH_METRICS_H_
#define COMPONENTS_AUTOFILL_CORE_BROWSER_METRICS_PAYMENTS_MANDATORY_REAUTH_METRICS_H_

#include "components/autofill/core/browser/autofill_client.h"
#include "components/autofill/core/browser/metrics/autofill_metrics.h"

namespace autofill::autofill_metrics {

// These values are persisted to logs. Entries should not be renumbered and
// numeric values should never be reused.
enum class MandatoryReauthOptInBubbleOffer {
  // The user is shown the opt-in bubble.
  kShown = 0,
  kMaxValue = kShown,
};

// These values are persisted to logs. Entries should not be renumbered and
// numeric values should never be reused.
enum class MandatoryReauthOptInBubbleResult {
  // The reason why the bubble is closed is not clear. Possible reason is the
  // logging function is invoked before the closed reason is correctly set.
  kUnknown = 0,
  // The user explicitly accepted the bubble by clicking the ok button.
  kAccepted = 1,
  // The user explicitly cancelled the bubble by clicking the cancel button.
  kCancelled = 2,
  // The user explicitly closed the bubble with the close button or ESC.
  kClosed = 3,
  // The user did not interact with the bubble.
  kNotInteracted = 4,
  // The bubble lost focus and was deactivated.
  kLostFocus = 5,
  kMaxValue = kLostFocus,
};

enum class MandatoryReauthOptInConfirmationBubbleMetric {
  // The user is shown the opt-in confirmation bubble.
  kShown = 0,
  // The user clicks the settings link of the opt-in confirmation bubble.
  kSettingsLinkClicked = 1,
  kMaxValue = kSettingsLinkClicked,
};

// Enum class to include all the possible auth flows that can occur for
// mandatory reauth. These values are persisted to logs.
// Entries should not be renumbered and numeric values should never be reused.
enum class MandatoryReauthAuthenticationFlowEvent {
  kUnknown = 0,
  // User authentication flow started.
  kFlowStarted = 1,
  // User authentication flow succeeded.
  kFlowSucceeded = 2,
  // User authentication flow failed.
  kFlowFailed = 3,
  kMaxValue = kFlowFailed,
};

// All the sources that can trigger the OptIn or OptOut flow for mandatory
// reauth.
enum class MandatoryReauthOptInOrOutSource {
  kUnknown = 0,
  // The OptIn or OptOut process is triggered from the settings page.
  kSettingsPage = 1,
  kMaxValue = kSettingsPage,
};

// Logs when the user is offered mandatory reauth.
void LogMandatoryReauthOptInBubbleOffer(MandatoryReauthOptInBubbleOffer metric,
                                        bool is_reshow);

// Logs when the user interacts with the opt-in bubble.
void LogMandatoryReauthOptInBubbleResult(
    MandatoryReauthOptInBubbleResult metric,
    bool is_reshow);

// Logs events related to the opt-in confirmation bubble.
void LogMandatoryReauthOptInConfirmationBubbleMetric(
    MandatoryReauthOptInConfirmationBubbleMetric metric);

// Logs all the possible flows for mandatory reauth during OptIn or OptOut
// process.
// We check the status of the mandatory reauth feature to determine if the
// user is trying to opt in or out.
// If mandatory reauth is currently on, and the user is trying to turn it off
// then the bool `opt_in` will be false.
// If mandatory reauth is currently off, and the user is trying to turn it on
// then the bool `opt_in` will be true.
void LogMandatoryReauthOptInOrOutUpdateEvent(
    MandatoryReauthOptInOrOutSource source,
    bool opt_in,
    MandatoryReauthAuthenticationFlowEvent event);

}  // namespace autofill::autofill_metrics

#endif  // COMPONENTS_AUTOFILL_CORE_BROWSER_METRICS_PAYMENTS_MANDATORY_REAUTH_METRICS_H_
