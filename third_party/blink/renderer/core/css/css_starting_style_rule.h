// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_STARTING_STYLE_RULE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_STARTING_STYLE_RULE_H_

#include "third_party/blink/renderer/core/css/css_grouping_rule.h"
#include "third_party/blink/renderer/platform/wtf/casting.h"

namespace blink {

class StyleRuleStartingStyle;

class CSSStartingStyleRule final : public CSSGroupingRule {
  DEFINE_WRAPPERTYPEINFO();

 public:
  CSSStartingStyleRule(StyleRuleStartingStyle*, CSSStyleSheet*);
  ~CSSStartingStyleRule() override = default;

  String cssText() const override;

 private:
  CSSRule::Type GetType() const override { return kStartingStyleRule; }
};

template <>
struct DowncastTraits<CSSStartingStyleRule> {
  static bool AllowFrom(const CSSRule& rule) {
    return rule.GetType() == CSSRule::kStartingStyleRule;
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_STARTING_STYLE_RULE_H_
