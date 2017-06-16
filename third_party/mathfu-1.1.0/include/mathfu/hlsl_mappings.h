/*
* Copyright 2017 Google Inc. All rights reserved.
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
#ifndef MATHFU_HLSL_MAPPINGS_H_
#define MATHFU_HLSL_MAPPINGS_H_

#include "mathfu/matrix.h"
#include "mathfu/quaternion.h"
#include "mathfu/vector.h"

/// @file mathfu/hlsl_mappings.h
/// @brief HLSL compatible data types.
/// @addtogroup mathfu_hlsl
///
/// To simplify the use of MathFu template classes and make it possible to
/// write code that looks similar to
/// <a href="https://msdn.microsoft.com/en-us/library/windows/desktop/bb509587(v=vs.85).aspx">HLSL</a> data types in C++,
/// MathFu provides a set of data types that are similar in style to
/// HLSL Vector and Matrix data types.

/// @brief Namespace for MathFu library.
namespace mathfu {

/// @addtogroup mathfu_hlsl
/// @{

/// Scalar unsigned integer
typedef unsigned int   uint;
typedef unsigned int   dword;
typedef unsigned short half;

/// 2-dimensional <code>float</code> Vector.
typedef Vector<float, 2> float2;
/// 3-dimensional <code>float</code> Vector.
typedef Vector<float, 3> float3;
/// 4-dimensional <code>float</code> Vector.
typedef Vector<float, 4> float4;

/// 2-dimensional <code>int</code> Vector.
typedef Vector<int, 2> int2;
/// 3-dimensional <code>int</code> Vector.
typedef Vector<int, 3> int3;
/// 4-dimensional <code>int</code> Vector.
typedef Vector<int, 4> int4;

/// 2-dimensional <code>uint</code> Vector.
typedef Vector<uint, 2> uint2;
/// 3-dimensional <code>uint</code> Vector.
typedef Vector<uint, 3> uint3;
/// 4-dimensional <code>uint</code> Vector.
typedef Vector<uint, 4> uint4;

/// 1x1 <code>float</code> Matrix.
typedef Matrix<float, 2, 2> float1x1;
/// 2x2 <code>float</code> Matrix.
typedef Matrix<float, 2, 2> float2x2;
/// 3x3 <code>float</code> Matrix.
typedef Matrix<float, 3, 3> float3x3;
/// 3x3 <code>float</code> Matrix.
typedef Matrix<float, 4, 4> float4x4;

/// 1x1 <code>double</code> Matrix.
typedef Matrix<double, 2, 2> double1x1;
/// 2x2 <code>double</code> Matrix.
typedef Matrix<double, 2, 2> double2x2;
/// 3x3 <code>double</code> Matrix.
typedef Matrix<double, 3, 3> double3x3;
/// 3x3 <code>double</code> Matrix.
typedef Matrix<double, 4, 4> double4x4;

/// 1x1 <code>int</code> Matrix.
typedef Matrix<int, 2, 2> int1x1;
/// 2x2 <code>int</code> Matrix.
typedef Matrix<int, 2, 2> int2x2;
/// 3x3 <code>int</code> Matrix.
typedef Matrix<int, 3, 3> int3x3;
/// 3x3 <code>int</code> Matrix.
typedef Matrix<int, 4, 4> int4x4;

/// 1x1 <code>int</code> Matrix.
typedef Matrix<int, 2, 2> uint1x1;
/// 2x2 <code>int</code> Matrix.
typedef Matrix<int, 2, 2> uint2x2;
/// 3x3 <code>int</code> Matrix.
typedef Matrix<int, 3, 3> uint3x3;
/// 3x3 <code>int</code> Matrix.
typedef Matrix<int, 4, 4> uint4x4;

/// @brief Calculate the cross product of two 3-dimensional Vectors.
///
/// @param v1 Vector to multiply
/// @param v2 Vector to multiply
/// @return 3-dimensional vector that contains the result.
template<class T>
inline Vector<T, 3> cross(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
  return Vector<T, 3>::CrossProduct(v1,v2);
}

/// @brief Calculate the dot product of two N-dimensional Vectors of any type.
///
/// @param v1 Vector to multiply
/// @param v2 Vector to multiply
/// @return Scalar dot product result.
template<class TV>
inline typename TV::Scalar dot(const TV& v1, const TV& v2) {
  return TV::DotProduct(v1,v2);
}

/// @brief Normalize an N-dimensional Vector of an arbitrary type.
///
/// @param v1 Vector to normalize.
/// @return Normalized vector.
template<class TV>
inline TV normalize(const TV& v1) {
  return v1.Normalized();
}

/// @}

}  // namespace mathfu

#endif  // MATHFU_HLSL_MAPPINGS_H_
