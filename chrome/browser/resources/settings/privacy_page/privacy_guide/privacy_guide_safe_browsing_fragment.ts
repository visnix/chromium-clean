// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @fileoverview
 * 'privacy-guide-safe-browsing-fragment' is the fragment in a privacy
 * guide card that contains the safe browsing settings and their descriptions.
 */
import 'chrome://resources/cr_components/settings_prefs/prefs.js';
import './privacy_guide_description_item.js';
import './privacy_guide_fragment_shared.css.js';
import '/shared/settings/controls/settings_radio_group.js';
import '../../privacy_page/collapse_radio_button.js';

import {PrefsMixin} from 'chrome://resources/cr_components/settings_prefs/prefs_mixin.js';
import {I18nMixin} from 'chrome://resources/cr_elements/i18n_mixin.js';
import {PolymerElement} from 'chrome://resources/polymer/v3_0/polymer/polymer_bundled.min.js';

import {loadTimeData} from '../../i18n_setup.js';
import {MetricsBrowserProxy, MetricsBrowserProxyImpl, PrivacyGuideSettingsStates, PrivacyGuideStepsEligibleAndReached} from '../../metrics_browser_proxy.js';
import {SafeBrowsingSetting} from '../../privacy_page/security_page.js';

import {getTemplate} from './privacy_guide_safe_browsing_fragment.html.js';


const PrivacyGuideSafeBrowsingFragmentBase =
    I18nMixin(PrefsMixin(PolymerElement));

export class PrivacyGuideSafeBrowsingFragmentElement extends
    PrivacyGuideSafeBrowsingFragmentBase {
  static get is() {
    return 'privacy-guide-safe-browsing-fragment';
  }

  static get template() {
    return getTemplate();
  }

  static get properties() {
    return {
      /**
       * Preferences state.
       */
      prefs: {
        type: Object,
        notify: true,
      },

      /**
       * Valid safe browsing states.
       */
      safeBrowsingSettingEnum_: {
        type: Object,
        value: SafeBrowsingSetting,
      },

      enableFriendlierSafeBrowsingSettingsStandardProtection_: {
        type: Boolean,
        value() {
          return loadTimeData.getBoolean(
              'enableFriendlierSafeBrowsingSettingsStandardProtection');
        },
      },
    };
  }

  private metricsBrowserProxy_: MetricsBrowserProxy =
      MetricsBrowserProxyImpl.getInstance();
  private startStateEnhanced_: boolean;
  private enableFriendlierSafeBrowsingSettingsStandardProtection_: boolean;

  override ready() {
    super.ready();
    this.addEventListener('view-enter-start', this.onViewEnterStart_);
    this.addEventListener('view-exit-finish', this.onViewExitFinish_);
  }

  override focus() {
    this.shadowRoot!.querySelector<HTMLElement>('[focus-element]')!.focus();
  }

  private onViewEnterStart_() {
    this.startStateEnhanced_ = this.getPref('generated.safe_browsing').value ===
        SafeBrowsingSetting.ENHANCED;
    this.metricsBrowserProxy_
        .recordPrivacyGuideStepsEligibleAndReachedHistogram(
            PrivacyGuideStepsEligibleAndReached.SAFE_BROWSING_REACHED);
  }

  private onViewExitFinish_() {
    const endStateEnhanced = this.getPref('generated.safe_browsing').value ===
        SafeBrowsingSetting.ENHANCED;

    let state: PrivacyGuideSettingsStates|null = null;
    if (this.startStateEnhanced_) {
      state = endStateEnhanced ?
          PrivacyGuideSettingsStates.SAFE_BROWSING_ENHANCED_TO_ENHANCED :
          PrivacyGuideSettingsStates.SAFE_BROWSING_ENHANCED_TO_STANDARD;
    } else {
      state = endStateEnhanced ?
          PrivacyGuideSettingsStates.SAFE_BROWSING_STANDARD_TO_ENHANCED :
          PrivacyGuideSettingsStates.SAFE_BROWSING_STANDARD_TO_STANDARD;
    }
    this.metricsBrowserProxy_.recordPrivacyGuideSettingsStatesHistogram(state!);
  }

  private onSafeBrowsingEnhancedClick_() {
    this.metricsBrowserProxy_.recordAction(
        'Settings.PrivacyGuide.ChangeSafeBrowsingEnhanced');
  }

  private onSafeBrowsingStandardClick_() {
    this.metricsBrowserProxy_.recordAction(
        'Settings.PrivacyGuide.ChangeSafeBrowsingStandard');
  }

  private onRadioGroupKeyDown_(event: KeyboardEvent) {
    switch (event.key) {
      case 'ArrowLeft':
      case 'ArrowRight':
        // This event got consumed by the radio group to change the radio button
        // selection. Do not propagate further, to not cause a privacy guide
        // navigation.
        event.stopPropagation();
        break;
    }
  }

  private getSafeBrowsingStandardSubLabel_(): string {
    return this.i18n(
        this.enableFriendlierSafeBrowsingSettingsStandardProtection_ ?
            'safeBrowsingStandardDescUpdated' :
            'safeBrowsingStandardDesc');
  }
}

declare global {
  interface HTMLElementTagNameMap {
    'privacy-guide-safe-browsing-fragment':
        PrivacyGuideSafeBrowsingFragmentElement;
  }
}

customElements.define(
    PrivacyGuideSafeBrowsingFragmentElement.is,
    PrivacyGuideSafeBrowsingFragmentElement);
