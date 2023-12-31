// Copyright 2023 The Crashpad Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CRASHPAD_UTIL_LINUX_PAC_HELPER_H_
#define CRASHPAD_UTIL_LINUX_PAC_HELPER_H_

#include "util/misc/address_types.h"

namespace crashpad {

//! \brief Strips PAC bits from an address
VMAddress StripPACBits(VMAddress address);

}  // namespace crashpad


#endif  // CRASHPAD_UTIL_LINUX_PAC_HELPER_H_

