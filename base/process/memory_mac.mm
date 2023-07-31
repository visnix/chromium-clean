// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/memory.h"

#include <new>

#include "base/allocator/buildflags.h"
#include "base/allocator/partition_allocator/partition_alloc_buildflags.h"
#include "base/allocator/partition_allocator/shim/allocator_interception_mac.h"
#include "base/allocator/partition_allocator/shim/allocator_shim.h"
#include "build/build_config.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace base {

namespace {
void oom_killer_new() {
  TerminateBecauseOutOfMemory(0);
}
}  // namespace

void EnableTerminationOnHeapCorruption() {
#if !ARCH_CPU_64_BITS
  DLOG(WARNING) << "EnableTerminationOnHeapCorruption only works on 64-bit";
#endif
}

bool UncheckedMalloc(size_t size, void** result) {
#if BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)
  // Unchecked allocations can happen before the default malloc() zone is
  // registered. In this case, going straight to the shim may explode, since the
  // memory will come from a zone which is unknown to the dispatching code in
  // libmalloc. Meaning that if the memory gets free()-d, realloc()-ed, or its
  // actual size is queried with malloc_size() *before* we get to register our
  // zone, we crash.
  //
  // The cleanest solution would be to detect it and forbid it, but tests (at
  // least) allocate in static constructors. Meaning that this code is
  // sufficient to cause a crash:
  //
  // void* ptr = []() {
  //  void* ptr;
  //  bool ok = base::UncheckedMalloc(1000, &ptr);
  //  CHECK(ok);
  //  free(ptr);
  // }();
  //
  // (Our static initializer is supposed to have priority, but it doesn't seem
  // to work in practice, at least for MachO).
  //
  // Since unchecked allocations are rare, let's err on the side of caution.
  if (!allocator_shim::IsDefaultAllocatorPartitionRootInitialized()) {
    *result = malloc(size);
    return *result != nullptr;
  }

  // Unlike use_partition_alloc_as_malloc=false, the default malloc zone is
  // replaced with PartitionAlloc, so the allocator shim functions work best.
  *result = allocator_shim::UncheckedAlloc(size);
  return *result != nullptr;
#else   // BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)
  return allocator_shim::UncheckedMallocMac(size, result);
#endif  // BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)
}

// The standard version is defined in memory.cc in case of
// USE_PARTITION_ALLOC_AS_MALLOC.
#if !BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)
bool UncheckedCalloc(size_t num_items, size_t size, void** result) {
  return allocator_shim::UncheckedCallocMac(num_items, size, result);
}
#endif  // !BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)

void EnableTerminationOnOutOfMemory() {
  // Step 1: Enable OOM killer on C++ failures.
  std::set_new_handler(oom_killer_new);

// Step 2: Enable OOM killer on C-malloc failures for the default zone (if we
// have a shim).
#if BUILDFLAG(USE_ALLOCATOR_SHIM)
  allocator_shim::SetCallNewHandlerOnMallocFailure(true);
#endif

  // Step 3: Enable OOM killer on all other malloc zones (or just "all" without
  // "other" if shim is disabled).
  allocator_shim::InterceptAllocationsMac();
}

void UncheckedFree(void* ptr) {
#if BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)
  // Important: might be different from free(), because in some cases, free()
  // does not necessarily know about allocator_shim::* functions.
  allocator_shim::UncheckedFree(ptr);
#else   // BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)
  free(ptr);
#endif  // BUILDFLAG(USE_PARTITION_ALLOC_AS_MALLOC)
}

}  // namespace base