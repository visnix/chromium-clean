// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ALLOCATOR_PARTITION_ALLOCATOR_PARTITION_ALLOC_BASE_CXX17_BACKPORTS_H_
#define BASE_ALLOCATOR_PARTITION_ALLOCATOR_PARTITION_ALLOC_BASE_CXX17_BACKPORTS_H_

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "base/allocator/partition_allocator/partition_alloc_base/check.h"

namespace partition_alloc::internal::base {

// C++14 implementation of C++17's std::clamp():
// https://en.cppreference.com/w/cpp/algorithm/clamp
// Please note that the C++ spec makes it undefined behavior to call std::clamp
// with a value of `lo` that compares greater than the value of `hi`. This
// implementation uses a CHECK to enforce this as a hard restriction.
template <typename T, typename Compare>
constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp) {
  PA_BASE_CHECK(!comp(hi, lo));
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
  return base::clamp(v, lo, hi, std::less<T>{});
}

}  // namespace partition_alloc::internal::base

#endif  // BASE_ALLOCATOR_PARTITION_ALLOCATOR_PARTITION_ALLOC_BASE_CXX17_BACKPORTS_H_
