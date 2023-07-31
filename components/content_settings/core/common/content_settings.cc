// Copyright 2011 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/content_settings/core/common/content_settings.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "base/check_op.h"
#include "base/containers/fixed_flat_map.h"
#include "base/metrics/histogram_functions.h"
#include "base/notreached.h"
#include "build/build_config.h"
#include "components/content_settings/core/common/content_settings_metadata.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/content_settings/core/common/content_settings_utils.h"

namespace {

// WARNING: The value specified here for a type should match exactly the value
// specified in the ContentType enum in enums.xml. Since these values are
// used for histograms, please do not reuse the same value for a different
// content setting. Always append to the end and increment.
constexpr auto kHistogramValue = base::MakeFixedFlatMap<ContentSettingsType,
                                                        int>({
    // Cookies was previously logged to bucket 0, which is not a valid bucket
    // for linear histograms.
    {ContentSettingsType::COOKIES, 100},
    {ContentSettingsType::IMAGES, 1},
    {ContentSettingsType::JAVASCRIPT, 2},
    // Removed PLUGINS in M91.
    {ContentSettingsType::POPUPS, 4},
    {ContentSettingsType::GEOLOCATION, 5},
    {ContentSettingsType::NOTIFICATIONS, 6},
    {ContentSettingsType::AUTO_SELECT_CERTIFICATE, 7},
    {ContentSettingsType::MIXEDSCRIPT, 10},
    {ContentSettingsType::MEDIASTREAM_MIC, 12},
    {ContentSettingsType::MEDIASTREAM_CAMERA, 13},
    {ContentSettingsType::PROTOCOL_HANDLERS, 14},
    // PPAPI_BROKER is deprecated and shouldn't get logged anymore.
    {ContentSettingsType::DEPRECATED_PPAPI_BROKER, -1},
    {ContentSettingsType::AUTOMATIC_DOWNLOADS, 16},
    {ContentSettingsType::MIDI_SYSEX, 17},
    {ContentSettingsType::SSL_CERT_DECISIONS, 19},
    {ContentSettingsType::PROTECTED_MEDIA_IDENTIFIER, 21},
    {ContentSettingsType::APP_BANNER, 22},
    {ContentSettingsType::SITE_ENGAGEMENT, 23},
    {ContentSettingsType::DURABLE_STORAGE, 24},
    // Removed "Key generation setting"
    {ContentSettingsType::BLUETOOTH_GUARD, 26},
    {ContentSettingsType::BACKGROUND_SYNC, 27},
    {ContentSettingsType::AUTOPLAY, 28},
    {ContentSettingsType::IMPORTANT_SITE_INFO, 30},
    {ContentSettingsType::PERMISSION_AUTOBLOCKER_DATA, 31},
    {ContentSettingsType::ADS, 32},
    {ContentSettingsType::ADS_DATA, 33},
    {ContentSettingsType::PASSWORD_PROTECTION, 34},
    {ContentSettingsType::MEDIA_ENGAGEMENT, 35},
    {ContentSettingsType::SOUND, 36},
    {ContentSettingsType::CLIENT_HINTS, 37},
    {ContentSettingsType::SENSORS, 38},
    {ContentSettingsType::ACCESSIBILITY_EVENTS, 39},
    {ContentSettingsType::PAYMENT_HANDLER, 43},
    {ContentSettingsType::USB_GUARD, 44},
    {ContentSettingsType::BACKGROUND_FETCH, 45},
    {ContentSettingsType::INTENT_PICKER_DISPLAY, 46},
    {ContentSettingsType::IDLE_DETECTION, 47},
    {ContentSettingsType::SERIAL_GUARD, 48},
    {ContentSettingsType::SERIAL_CHOOSER_DATA, 49},
    {ContentSettingsType::PERIODIC_BACKGROUND_SYNC, 50},
    {ContentSettingsType::BLUETOOTH_SCANNING, 51},
    {ContentSettingsType::HID_GUARD, 52},
    {ContentSettingsType::HID_CHOOSER_DATA, 53},
    {ContentSettingsType::WAKE_LOCK_SCREEN, 54},
    {ContentSettingsType::WAKE_LOCK_SYSTEM, 55},
    {ContentSettingsType::LEGACY_COOKIE_ACCESS, 56},
    {ContentSettingsType::FILE_SYSTEM_WRITE_GUARD, 57},
    // Removed INSTALLED_WEB_APP_METADATA in M107.
    {ContentSettingsType::NFC, 59},
    {ContentSettingsType::BLUETOOTH_CHOOSER_DATA, 60},
    {ContentSettingsType::CLIPBOARD_READ_WRITE, 61},
    {ContentSettingsType::CLIPBOARD_SANITIZED_WRITE, 62},
    {ContentSettingsType::SAFE_BROWSING_URL_CHECK_DATA, 63},
    {ContentSettingsType::VR, 64},
    {ContentSettingsType::AR, 65},
    {ContentSettingsType::FILE_SYSTEM_READ_GUARD, 66},
    {ContentSettingsType::STORAGE_ACCESS, 67},
    {ContentSettingsType::CAMERA_PAN_TILT_ZOOM, 68},
    {ContentSettingsType::WINDOW_MANAGEMENT, 69},
    {ContentSettingsType::INSECURE_PRIVATE_NETWORK, 70},
    {ContentSettingsType::LOCAL_FONTS, 71},
    {ContentSettingsType::PERMISSION_AUTOREVOCATION_DATA, 72},
    {ContentSettingsType::FILE_SYSTEM_LAST_PICKED_DIRECTORY, 73},
    {ContentSettingsType::DISPLAY_CAPTURE, 74},
    // Removed FILE_HANDLING in M98.
    {ContentSettingsType::FILE_SYSTEM_ACCESS_CHOOSER_DATA, 76},
    {ContentSettingsType::FEDERATED_IDENTITY_SHARING, 77},
    // Removed FEDERATED_IDENTITY_REQUEST in M103.
    {ContentSettingsType::JAVASCRIPT_JIT, 79},
    {ContentSettingsType::HTTP_ALLOWED, 80},
    {ContentSettingsType::FORMFILL_METADATA, 81},
    {ContentSettingsType::FEDERATED_IDENTITY_ACTIVE_SESSION, 82},
    {ContentSettingsType::AUTO_DARK_WEB_CONTENT, 83},
    {ContentSettingsType::REQUEST_DESKTOP_SITE, 84},
    {ContentSettingsType::FEDERATED_IDENTITY_API, 85},
    {ContentSettingsType::NOTIFICATION_INTERACTIONS, 86},
    {ContentSettingsType::REDUCED_ACCEPT_LANGUAGE, 87},
    {ContentSettingsType::NOTIFICATION_PERMISSION_REVIEW, 88},
    {ContentSettingsType::PRIVATE_NETWORK_GUARD, 89},
    {ContentSettingsType::PRIVATE_NETWORK_CHOOSER_DATA, 90},
    {ContentSettingsType::FEDERATED_IDENTITY_IDENTITY_PROVIDER_SIGNIN_STATUS,
     91},
    {ContentSettingsType::REVOKED_UNUSED_SITE_PERMISSIONS, 92},
    {ContentSettingsType::TOP_LEVEL_STORAGE_ACCESS, 93},
    {ContentSettingsType::FEDERATED_IDENTITY_AUTO_REAUTHN_PERMISSION, 94},
    {ContentSettingsType::FEDERATED_IDENTITY_IDENTITY_PROVIDER_REGISTRATION,
     95},
    {ContentSettingsType::ANTI_ABUSE, 96},
    {ContentSettingsType::THIRD_PARTY_STORAGE_PARTITIONING, 97},
    {ContentSettingsType::HTTPS_ENFORCED, 98},
    {ContentSettingsType::USB_CHOOSER_DATA, 99},
    // The value 100 is assigned to COOKIES!
    // Removed GET_DISPLAY_MEDIA_SET_SELECT_ALL_SCREENS in M116.
    {ContentSettingsType::MIDI, 102},
    {ContentSettingsType::ALL_SCREEN_CAPTURE, 103},
    {ContentSettingsType::COOKIE_CONTROLS_METADATA, 104},

    // As mentioned at the top, please don't forget to update ContentType in
    // enums.xml when you add entries here!
});

constexpr int kkHistogramValueMax = std::max_element(
    kHistogramValue.begin(),
    kHistogramValue.end(),
    [](const auto a, const auto b) { return a.second < b.second; }) -> second;

void FilterRulesForType(ContentSettingsForOneType& settings,
                        const GURL& primary_url) {
  base::EraseIf(settings,
                [&primary_url](const ContentSettingPatternSource& source) {
                  return !source.primary_pattern.Matches(primary_url);
                });
  // We should have at least on rule remaining (the default rule).
  DCHECK_GE(settings.size(), 1u);
}

}  // namespace

ContentSetting IntToContentSetting(int content_setting) {
  return ((content_setting < 0) ||
          (content_setting >= CONTENT_SETTING_NUM_SETTINGS))
             ? CONTENT_SETTING_DEFAULT
             : static_cast<ContentSetting>(content_setting);
}

void RecordContentSettingsHistogram(const char* name,
                                    ContentSettingsType content_setting) {
  base::UmaHistogramExactLinear(
      name, ContentSettingTypeToHistogramValue(content_setting),
      kkHistogramValueMax + 1);
}

int ContentSettingTypeToHistogramValue(ContentSettingsType content_setting) {
  static_assert(kHistogramValue.size() ==
                    static_cast<size_t>(ContentSettingsType::NUM_TYPES),
                "Update content settings histogram lookup");

  auto* found = kHistogramValue.find(content_setting);
  if (found != kHistogramValue.end()) {
    DCHECK_NE(found->second, -1)
        << "Used for deprecated settings: " << static_cast<int>(found->first);
    return found->second;
  }
  NOTREACHED();
  return -1;
}

ContentSettingPatternSource::ContentSettingPatternSource(
    const ContentSettingsPattern& primary_pattern,
    const ContentSettingsPattern& secondary_pattern,
    base::Value setting_value,
    const std::string& source,
    bool incognito,
    content_settings::RuleMetaData metadata)
    : primary_pattern(primary_pattern),
      secondary_pattern(secondary_pattern),
      setting_value(std::move(setting_value)),
      metadata(metadata),
      source(source),
      incognito(incognito) {}

ContentSettingPatternSource::ContentSettingPatternSource() : incognito(false) {}

ContentSettingPatternSource::ContentSettingPatternSource(
    const ContentSettingPatternSource& other) {
  *this = other;
}

ContentSettingPatternSource& ContentSettingPatternSource::operator=(
    const ContentSettingPatternSource& other) {
  primary_pattern = other.primary_pattern;
  secondary_pattern = other.secondary_pattern;
  setting_value = other.setting_value.Clone();
  metadata = other.metadata;
  source = other.source;
  incognito = other.incognito;
  return *this;
}

ContentSettingPatternSource::~ContentSettingPatternSource() = default;

ContentSetting ContentSettingPatternSource::GetContentSetting() const {
  return content_settings::ValueToContentSetting(setting_value);
}

bool ContentSettingPatternSource::IsExpired() const {
  return !metadata.expiration().is_null() &&
         metadata.expiration() < base::Time::Now();
}

bool ContentSettingPatternSource::operator==(
    const ContentSettingPatternSource& other) const {
  return std::tie(primary_pattern, secondary_pattern, setting_value, metadata,
                  source, incognito) ==
         std::tie(other.primary_pattern, other.secondary_pattern,
                  other.setting_value, other.metadata, other.source,
                  other.incognito);
}

// static
bool RendererContentSettingRules::IsRendererContentSetting(
    ContentSettingsType content_type) {
  return content_type == ContentSettingsType::IMAGES ||
         content_type == ContentSettingsType::JAVASCRIPT ||
         content_type == ContentSettingsType::POPUPS ||
         content_type == ContentSettingsType::MIXEDSCRIPT ||
         content_type == ContentSettingsType::AUTO_DARK_WEB_CONTENT;
}

void RendererContentSettingRules::FilterRulesByOutermostMainFrameURL(
    const GURL& outermost_main_frame_url) {
  FilterRulesForType(image_rules, outermost_main_frame_url);
  FilterRulesForType(script_rules, outermost_main_frame_url);
  FilterRulesForType(popup_redirect_rules, outermost_main_frame_url);
  FilterRulesForType(mixed_content_rules, outermost_main_frame_url);
  FilterRulesForType(auto_dark_content_rules, outermost_main_frame_url);
}

RendererContentSettingRules::RendererContentSettingRules() = default;

RendererContentSettingRules::~RendererContentSettingRules() = default;

RendererContentSettingRules::RendererContentSettingRules(
    const RendererContentSettingRules&) = default;

RendererContentSettingRules::RendererContentSettingRules(
    RendererContentSettingRules&& rules) = default;

RendererContentSettingRules& RendererContentSettingRules::operator=(
    const RendererContentSettingRules& rules) = default;

RendererContentSettingRules& RendererContentSettingRules::operator=(
    RendererContentSettingRules&& rules) = default;

bool RendererContentSettingRules::operator==(
    const RendererContentSettingRules& other) const {
  return std::tie(image_rules, script_rules, popup_redirect_rules,
                  mixed_content_rules, auto_dark_content_rules) ==
         std::tie(other.image_rules, other.script_rules,
                  other.popup_redirect_rules, other.mixed_content_rules,
                  other.auto_dark_content_rules);
}
