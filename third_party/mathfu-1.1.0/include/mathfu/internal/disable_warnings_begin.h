/*
* Copyright 2016 Google Inc. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

// Suppress pedantic warnings.
// To re-enable, include "mathfu/internal/disable_warnings_end.h".
//
// We need this to use anonymous unions and structs, which generate the
// the following warning in GCC and Clang,
//     error: ISO C++ prohibits anonymous structs [-Werror=pedantic]
// The only way to suppress this warning is to turn off all pedantic warnings.

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#endif  // defined(__GNUC__)

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#endif  // defined(__clang__)
