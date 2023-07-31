// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SANITIZER_API_BUILTINS_SANITIZER_BUILTINS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SANITIZER_API_BUILTINS_SANITIZER_BUILTINS_H_

// Access Sanitizer API constants.
//
// The constant values are generated by generate_builtins.py.
// Generate with:
//  ninja -C ... generate_sanitizer_builtins generate_sanitizer_attribute_lists

namespace blink {

extern const char* const kBaselineElements[];
extern const char* const kBaselineAttributes[];
extern const char* const kDefaultElements[];
extern const char* const kDefaultAttributes[];
extern const char* const kKnownAttributes[];

// We currently support an alternate set of builtins, enabled via a flag.
namespace with_namespace_names {
extern const char* const kBaselineElements[];
extern const char* const kBaselineAttributes[];
extern const char* const kDefaultElements[];
extern const char* const kDefaultAttributes[];
extern const char* const kKnownAttributes[];
}  // namespace with_namespace_names
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SANITIZER_API_BUILTINS_SANITIZER_BUILTINS_H_