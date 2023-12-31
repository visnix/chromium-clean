// Copyright 2015 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/common/password_manager_features.h"

#include "base/feature_list.h"
#include "build/blink_buildflags.h"
#include "build/build_config.h"

namespace password_manager::features {
// NOTE: It is strongly recommended to use UpperCamelCase style for feature
//       names, e.g. "MyGreatFeature".

// Enables the overwriting of prefilled username fields if the server predicted
// the field to contain a placeholder value.
BASE_FEATURE(kEnableOverwritingPlaceholderUsernames,
             "EnableOverwritingPlaceholderUsernames",
             base::FEATURE_DISABLED_BY_DEFAULT);

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
// When enabled, initial sync will be forced during startup if the password
// store has encryption service failures.
BASE_FEATURE(kForceInitialSyncWhenDecryptionFails,
             "ForceInitialSyncWhenDecryptionFails",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif

// When enabled, username fields for forgot password flows are recognized
// and filled.
BASE_FEATURE(kForgotPasswordFormSupport,
             "ForgotPasswordFormSupport",
             base::FEATURE_DISABLED_BY_DEFAULT);

#if BUILDFLAG(IS_IOS)
// Removes the list of passwords from the Settings UI and adds a separate
// Password Manager view.
BASE_FEATURE(kIOSPasswordUISplit,
             "IOSPasswordUISplit",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables displaying and managing compromised, weak and reused credentials in
// the Password Manager.
BASE_FEATURE(kIOSPasswordCheckup,
             "IOSPasswordCheckup",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables password bottom sheet to be displayed (on iOS) when a user is
// signed-in and taps on a username or password field on a website that has at
// least one credential saved in their password manager.
BASE_FEATURE(kIOSPasswordBottomSheet,
             "IOSPasswordBottomSheet",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif  // IS_IOS

// Enables new regex for OTP fields.
BASE_FEATURE(kNewRegexForOtpFields,
             "NewRegexForOtpFields",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Killswitch for changes regarding password issues in
// `PasswordSpcificsMetadata`. Guards writing issues to metadata and preserving
// the new notification field.
BASE_FEATURE(kPasswordIssuesInSpecificsMetadata,
             "PasswordIssuesInSpecificsMetadata",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables sending credentials from the settings UI.
BASE_FEATURE(kSendPasswords,
             "SendPasswords",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables .well-known based password change flow from leaked password dialog.
BASE_FEATURE(kPasswordChangeWellKnown,
             "PasswordChangeWellKnown",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPasswordsImportM2,
             "PasswordsImportM2",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables password reuse detection.
BASE_FEATURE(kPasswordReuseDetectionEnabled,
             "PasswordReuseDetectionEnabled",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables different experiments that modify content and behavior of the
// existing generated password suggestion dropdown.
#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)  // Desktop
BASE_FEATURE(kPasswordGenerationExperiment,
             "PasswordGenerationExperiment",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif

// Enables showing UI which allows users to easily revert their choice to
// never save passwords on a certain website.
BASE_FEATURE(kRecoverFromNeverSaveAndroid,
             "RecoverFromNeverSaveAndroid_LAUNCHED",
             base::FEATURE_ENABLED_BY_DEFAULT);

#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)
// Enables a revamped version of the password management bubble triggered by
// manually clicking on the key icon in the omnibox.
BASE_FEATURE(kRevampedPasswordManagementBubble,
             "RevampedPasswordManagementBubble",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
// Displays at least the decryptable and never saved logins in the password
// manager
BASE_FEATURE(kSkipUndecryptablePasswords,
             "SkipUndecryptablePasswords",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif

#if BUILDFLAG(IS_ANDROID)
// Use GMS AccountSettings to manage passkeys when UPM is not available.
BASE_FEATURE(kPasskeyManagementUsingAccountSettingsAndroid,
             "PasskeyManagementUsingAccountSettingsAndroid",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kPasswordEditDialogWithDetails,
             "PasswordEditDialogWithDetails",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables the Password generation bottom sheet.
BASE_FEATURE(kPasswordGenerationBottomSheet,
             "PasswordGenerationBottomSheet",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables the refactored Password Suggestion bottom sheet (Touch-To-Fill).
// The goal of the refactoring is to transfer the knowledge about the
// Touch-To-Fill feature to the browser code completely and so to simplify the
// renderer code. In the refactored version it will be decided inside the the
// `ContentPasswordManagerDriver::ShowPasswordSuggestions` whether to show the
// TTF to the user.
BASE_FEATURE(kPasswordSuggestionBottomSheetV2,
             "PasswordSuggestionBottomSheetV2",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables the intent fetching for the credential manager in Google Mobile
// Services. It does not enable launching the credential manager.
BASE_FEATURE(kUnifiedCredentialManagerDryRun,
             "UnifiedCredentialManagerDryRun",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables use of Google Mobile Services for password storage. Chrome's local
// database will be unused but kept in sync for local passwords.
BASE_FEATURE(kUnifiedPasswordManagerAndroid,
             "UnifiedPasswordManagerAndroid_LAUNCHED",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables use of Google Mobile services for non-sycned password storage.
BASE_FEATURE(kUnifiedPasswordManagerLocalPasswordsAndroid,
             "UnifiedPasswordManagerLocalPasswordsAndroid",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables showing the warning about UPM migrating local passwords.
BASE_FEATURE(kUnifiedPasswordManagerLocalPasswordsMigrationWarning,
             "UnifiedPasswordManagerLocalPasswordsMigrationWarning",
             base::FEATURE_DISABLED_BY_DEFAULT);

// If enabled, the built-in sync functionality in PasswordSyncBridge becomes
// unused, meaning that SyncService/SyncEngine will no longer download or
// upload changes to/from the Sync server. Instead, an external Android-specific
// backend will be used to achieve similar behavior.
BASE_FEATURE(kUnifiedPasswordManagerSyncUsingAndroidBackendOnly,
             "UnifiedPasswordManagerSyncUsingAndroidBackendOnly",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables all UI branding changes related to Unified Password Manager:
// the strings containing 'Password Manager' and the password manager
// icon.
BASE_FEATURE(kUnifiedPasswordManagerAndroidBranding,
             "UnifiedPasswordManagerAndroidBranding",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables new exploratory strings for the save/update password prompts.
BASE_FEATURE(kExploratorySaveUpdatePasswordStrings,
             "ExploratorySaveUpdatePasswordStrings",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPasswordsInCredMan,
             "PasswordsInCredMan",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif

// Enables support of sending additional votes on username first flow. The votes
// are sent on single password forms and contain information about preceding
// single username forms.
// TODO(crbug.com/959776): Clean up if the main crowdsourcing is good enough and
// we don't need additional signals.
BASE_FEATURE(kUsernameFirstFlowFallbackCrowdsourcing,
             "UsernameFirstFlowFallbackCrowdsourcing",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables suggesting username in the save/update prompt in the case of
// autocomplete="username".
BASE_FEATURE(kUsernameFirstFlowHonorAutocomplete,
             "UsernameFirstFlowHonorAutocomplete",
             base::FEATURE_DISABLED_BY_DEFAULT);

#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_IOS)  // Desktop
// Show, update, and delete GPM passkeys on the Chrome Password Manager.
BASE_FEATURE(kPasswordManagerPasskeys,
             "PasswordManagerPasskeys",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif

#if BUILDFLAG(IS_ANDROID)

// The string version to use for the save/update password prompts when the user
// is syncing passwords. Version 1 is outdated, so the only supported versions
// currently are 2 and 3.
extern const base::FeatureParam<int> kSaveUpdatePromptSyncingStringVersion = {
    &kExploratorySaveUpdatePasswordStrings, "syncing_string_version", 2};
#endif

// Field trial identifier for password generation requirements.
const char kGenerationRequirementsFieldTrial[] =
    "PasswordGenerationRequirements";

// The file version number of password requirements files. If the prefix length
// changes, this version number needs to be updated.
// Default to 0 in order to get an empty requirements file.
const char kGenerationRequirementsVersion[] = "version";

// Length of a hash prefix of domain names. This is used to shard domains
// across multiple files.
// Default to 0 in order to put all domain names into the same shard.
const char kGenerationRequirementsPrefixLength[] = "prefix_length";

// Timeout (in milliseconds) for password requirements lookups. As this is a
// network request in the background that does not block the UI, the impact of
// high values is not strong.
// Default to 5000 ms.
const char kGenerationRequirementsTimeout[] = "timeout";

#if BUILDFLAG(IS_ANDROID)
bool UsesUnifiedPasswordManagerUi() {
  if (!base::FeatureList::IsEnabled(kUnifiedPasswordManagerAndroid)) {
    return false;
  }
  UpmExperimentVariation variation = kUpmExperimentVariationParam.Get();
  switch (variation) {
    case UpmExperimentVariation::kEnableForSyncingUsers:
    case UpmExperimentVariation::kEnableForAllUsers:
      return true;
    case UpmExperimentVariation::kShadowSyncingUsers:
    case UpmExperimentVariation::kEnableOnlyBackendForSyncingUsers:
      return false;
  }
  NOTREACHED() << "Define explicitly whether UI is required!";
  return false;
}

bool UsesUnifiedPasswordManagerBranding() {
  return (UsesUnifiedPasswordManagerUi() ||
          base::FeatureList::IsEnabled(kUnifiedPasswordManagerAndroidBranding));
}

bool RequiresMigrationForUnifiedPasswordManager() {
  if (!base::FeatureList::IsEnabled(kUnifiedPasswordManagerAndroid)) {
    return false;
  }
  UpmExperimentVariation variation = kUpmExperimentVariationParam.Get();
  switch (variation) {
    case UpmExperimentVariation::kEnableForSyncingUsers:
    case UpmExperimentVariation::kEnableOnlyBackendForSyncingUsers:
    case UpmExperimentVariation::kEnableForAllUsers:
      return true;
    case UpmExperimentVariation::kShadowSyncingUsers:
      return false;
  }
  NOTREACHED() << "Define explicitly whether migration is required!";
  return false;
}

#endif  // IS_ANDROID

#if BUILDFLAG(IS_IOS)
bool IsPasswordCheckupEnabled() {
  return base::FeatureList::IsEnabled(
      password_manager::features::kIOSPasswordCheckup);
}
#endif  // IS_IOS

}  // namespace password_manager::features
