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
#ifndef MATHFU_GLSL_MAPPINGS_H_
#define MATHFU_GLSL_MAPPINGS_H_

#include "mathfu/matrix.h"
#include "mathfu/quaternion.h"
#include "mathfu/rect.h"
#include "mathfu/vector.h"

/// @file mathfu/glsl_mappings.h
/// @brief GLSL compatible data types.
/// @addtogroup mathfu_glsl
///
/// To simplify the use of MathFu template classes and make it possible to
/// write code that looks similar to
/// <a href="http://www.opengl.org/documentation/glsl/">GLSL</a> in C++,
/// MathFu provides a set of data types that are similar in style to
/// GLSL Vector and Matrix data types.

/// @brief Namespace for MathFu library.
namespace mathfu {

/// @addtogroup mathfu_glsl
/// @{

/// 2-dimensional <code>float</code> Vector.
typedef Vector<float, 2> vec2;
/// 3-dimensional <code>float</code> Vector.
typedef Vector<float, 3> vec3;
/// 4-dimensional <code>float</code> Vector.
typedef Vector<float, 4> vec4;

/// 2-dimensional <code>int</code> Vector.
typedef Vector<int, 2> vec2i;
/// 3-dimensional <code>int</code> Vector.
typedef Vector<int, 3> vec3i;
/// 4-dimensional <code>int</code> Vector.
typedef Vector<int, 4> vec4i;

/// 2x2 <code>float</code> Matrix.
typedef Matrix<float, 2, 2> mat2;
/// 3x3 <code>float</code> Matrix.
typedef Matrix<float, 3, 3> mat3;
/// 3x3 <code>float</code> Matrix.
typedef Matrix<float, 4, 4> mat4;

/// 2-dimensional <code>float</code> packed Vector (VectorPacked).
typedef VectorPacked<float, 2> vec2_packed;
/// 3-dimensional <code>float</code> packed Vector (VectorPacked).
typedef VectorPacked<float, 3> vec3_packed;
/// 4-dimensional <code>float</code> packed Vector (VectorPacked).
typedef VectorPacked<float, 4> vec4_packed;

/// 2-dimensional <code>int</code> packed Vector (VectorPacked).
typedef VectorPacked<int, 2> vec2i_packed;
/// 3-dimensional <code>int</code> packed Vector (VectorPacked).
typedef VectorPacked<int, 3> vec3i_packed;
/// 4-dimensional <code>int</code> packed Vector (VectorPacked).
typedef VectorPacked<int, 4> vec4i_packed;

/// Float-based quaternion.  Note that this is not technically
/// a GLES type, but is included for convenience.
typedef mathfu::Quaternion<float> quat;

/// Rect composed of type <code>float</code>.
typedef Rect<float> rectf;
/// Rect composed of type <code>double</code>.
typedef Rect<double> rectd;
/// Rect composed of type <code>int</code>.
typedef Rect<int> recti;

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

#endif  // MATHFU_GLSL_MAPPINGS_H_
