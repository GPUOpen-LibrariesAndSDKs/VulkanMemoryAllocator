/*
* Copyright 2014 Google Inc. All rights reserved.
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
#ifndef MATHFU_CONSTANTS_H
#define MATHFU_CONSTANTS_H

#include "mathfu/matrix.h"
#include "mathfu/quaternion.h"
#include "mathfu/vector.h"

namespace mathfu {

/// @file mathfu/constants.h
/// @brief Vector constants for specific dimensions.
/// @addtogroup mathfu_constants
///
/// It is preferable to use these constants rather than constructing them
/// when they're required. Construction most-likely slower than loading them
/// from memory.
/// <p>
/// For example, the following:<br>
/// <code>
/// lookat = mat4::LookAt(target, position, mathfu::kAxisY3f);
/// </code>
/// <br>is preferable to:<br>
/// <code>
/// lookat = mat4::LookAt(target, position,
///                       mathfu::Vector<float, 3>(0.0f, 1.0f, 0.0f));
/// </code>
/// <br> in terms of efficiency and in addition to resulting in more concise
/// code.
/// </p>
///
/// Depending on your linker's sophistication and settings, these constants may
/// be duplicated in every compilation unit in which they're used. However,
/// most linkers should be able to detect and eliminate this duplication.

/// @addtogroup mathfu_constants
/// @{

/// 2-dimensional <code>float</code> Vector of zeros.
static const Vector<float, 2> kZeros2f(0.0f, 0.0f);
/// 2-dimensional <code>float</code> Vector of ones.
static const Vector<float, 2> kOnes2f(1.0f, 1.0f);
/// 2-dimensional <code>float</code> unit Vector pointing along the X axis.
static const Vector<float, 2> kAxisX2f(1.0f, 0.0f);
/// 2-dimensional <code>float</code> unit Vector pointing along the Y axis.
static const Vector<float, 2> kAxisY2f(0.0f, 1.0f);

/// 3-dimensional <code>float</code> Vector of zeros.
static const Vector<float, 3> kZeros3f(0.0f, 0.0f, 0.0f);
/// 3-dimensional <code>float</code> Vector of ones.
static const Vector<float, 3> kOnes3f(1.0f, 1.0f, 1.0f);
/// 3-dimensional <code>float</code> unit Vector pointing along the X axis.
static const Vector<float, 3> kAxisX3f(1.0f, 0.0f, 0.0f);
/// 3-dimensional <code>float</code> unit Vector pointing along the Y axis.
static const Vector<float, 3> kAxisY3f(0.0f, 1.0f, 0.0f);
/// 3-dimensional <code>float</code> unit Vector pointing along the Z axis.
static const Vector<float, 3> kAxisZ3f(0.0f, 0.0f, 1.0f);

/// 4-dimensional <code>float</code> Vector of zeros.
static const Vector<float, 4> kZeros4f(0.0f, 0.0f, 0.0f, 0.0f);
/// 4-dimensional <code>float</code> Vector of ones.
static const Vector<float, 4> kOnes4f(1.0f, 1.0f, 1.0f, 1.0f);
/// 4-dimensional <code>float</code> unit Vector pointing along the X axis.
static const Vector<float, 4> kAxisX4f(1.0f, 0.0f, 0.0f, 0.0f);
/// 4-dimensional <code>float</code> unit Vector pointing along the Y axis.
static const Vector<float, 4> kAxisY4f(0.0f, 1.0f, 0.0f, 0.0f);
/// 4-dimensional <code>float</code> unit Vector pointing along the Z axis.
static const Vector<float, 4> kAxisZ4f(0.0f, 0.0f, 1.0f, 0.0f);
/// 4-dimensional <code>float</code> unit Vector pointing along the W axis.
static const Vector<float, 4> kAxisW4f(0.0f, 0.0f, 0.0f, 1.0f);

/// 2-dimensional <code>double</code> Vector of zeros.
static const Vector<double, 2> kZeros2d(0.0, 0.0);
/// 2-dimensional <code>double</code> Vector of ones.
static const Vector<double, 2> kOnes2d(1.0, 1.0);
/// 2-dimensional <code>double</code> unit Vector pointing along the X axis.
static const Vector<double, 2> kAxisX2d(1.0, 0.0);
/// 2-dimensional <code>double</code> unit Vector pointing along the Y axis.
static const Vector<double, 2> kAxisY2d(0.0, 1.0);

/// 3-dimensional <code>double</code> Vector of zeros.
static const Vector<double, 3> kZeros3d(0.0, 0.0, 0.0);
/// 3-dimensional <code>double</code> Vector of ones.
static const Vector<double, 3> kOnes3d(1.0, 1.0, 1.0);
/// 3-dimensional <code>double</code> unit Vector pointing along the X axis.
static const Vector<double, 3> kAxisX3d(1.0, 0.0, 0.0);
/// 3-dimensional <code>double</code> unit Vector pointing along the Y axis.
static const Vector<double, 3> kAxisY3d(0.0, 1.0, 0.0);
/// 3-dimensional <code>double</code> unit Vector pointing along the Z axis.
static const Vector<double, 3> kAxisZ3d(0.0, 0.0, 1.0);

/// 4-dimensional <code>double</code> Vector of zeros.
static const Vector<double, 4> kZeros4d(0.0, 0.0, 0.0, 0.0);
/// 4-dimensional <code>double</code> Vector of ones.
static const Vector<double, 4> kOnes4d(1.0, 1.0, 1.0, 1.0);
/// 4-dimensional <code>double</code> unit Vector pointing along the X axis.
static const Vector<double, 4> kAxisX4d(1.0, 0.0, 0.0, 0.0);
/// 4-dimensional <code>double</code> unit Vector pointing along the Y axis.
static const Vector<double, 4> kAxisY4d(0.0, 1.0, 0.0, 0.0);
/// 4-dimensional <code>double</code> unit Vector pointing along the Z axis.
static const Vector<double, 4> kAxisZ4d(0.0, 0.0, 1.0, 0.0);
/// 4-dimensional <code>double</code> unit Vector pointing along the W axis.
static const Vector<double, 4> kAxisW4d(0.0, 0.0, 0.0, 1.0);

/// 2-dimensional <code>int</code> Vector of zeros.
static const Vector<int, 2> kOnes2i(1, 1);
/// 2-dimensional <code>int</code> Vector of ones.
static const Vector<int, 2> kZeros2i(0, 0);
/// 2-dimensional <code>int</code> unit Vector pointing along the X axis.
static const Vector<int, 2> kAxisX2i(1, 0);
/// 2-dimensional <code>int</code> unit Vector pointing along the Y axis.
static const Vector<int, 2> kAxisY2i(0, 1);

/// 3-dimensional <code>int</code> Vector of zeros.
static const Vector<int, 3> kZeros3i(0, 0, 0);
/// 3-dimensional <code>int</code> Vector of ones.
static const Vector<int, 3> kOnes3i(1, 1, 1);
/// 3-dimensional <code>int</code> unit Vector pointing along the X axis.
static const Vector<int, 3> kAxisX3i(1, 0, 0);
/// 3-dimensional <code>int</code> unit Vector pointing along the Y axis.
static const Vector<int, 3> kAxisY3i(0, 1, 0);
/// 3-dimensional <code>int</code> unit Vector pointing along the Z axis.
static const Vector<int, 3> kAxisZ3i(0, 0, 1);

/// 4-dimensional <code>int</code> Vector of zeros.
static const Vector<int, 4> kZeros4i(0, 0, 0, 0);
/// 4-dimensional <code>int</code> Vector of ones.
static const Vector<int, 4> kOnes4i(1, 1, 1 ,1);
/// 4-dimensional <code>int</code> unit Vector pointing along the X axis.
static const Vector<int, 4> kAxisX4i(1, 0, 0, 0);
/// 4-dimensional <code>int</code> unit Vector pointing along the Z axis.
static const Vector<int, 4> kAxisY4i(0, 1, 0, 0);
/// 4-dimensional <code>int</code> unit Vector pointing along the Y axis.
static const Vector<int, 4> kAxisZ4i(0, 0, 1, 0);
/// 4-dimensional <code>int</code> unit Vector pointing along the W axis.
static const Vector<int, 4> kAxisW4i(0, 0, 0, 1);

/// Quaternion Identity
static const Quaternion<float> kQuatIdentityf(1.0f, 0.0f, 0.0f, 0.0f);
/// Quaternion Identity
static const Quaternion<double> kQuatIdentityd(1.0, 0.0, 0.0, 0.0);

// An AffineTransform versoin of the mat4 Identity matrix.
static const AffineTransform kAffineIdentity(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                                             0.0f);
/// @}

}  // namespace mathfu

#endif  // MATHFU_CONSTANTS_H
