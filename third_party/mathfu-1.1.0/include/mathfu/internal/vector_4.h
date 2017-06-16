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
#ifndef MATHFU_INTERNAL_VECTOR_4_H_
#define MATHFU_INTERNAL_VECTOR_4_H_

// Prefer including vector.h directly, since it includes specializations.
#include "mathfu/vector.h"

namespace mathfu {

template <class T>
class Vector<T, 4> {
 public:
  typedef T Scalar;
  static const int d = 4;

  inline Vector() {}

  inline Vector(const Vector<T, 4>& v) {
    MATHFU_VECTOR_OPERATION(data_[i] = v.data_[i]);
  }

  template <typename U>
  explicit inline Vector(const Vector<U, 4>& v) {
    MATHFU_VECTOR_OPERATION(data_[i] = static_cast<T>(v[i]));
  }

  explicit inline Vector(const T& s) { MATHFU_VECTOR_OPERATION(data_[i] = s); }

  explicit inline Vector(const T* a) {
    MATHFU_VECTOR_OPERATION(data_[i] = a[i]);
  }

  inline Vector(const T& s1, const T& s2, const T& s3, const T& s4) {
    x = s1;
    y = s2;
    z = s3;
    w = s4;
  }

  inline Vector(const Vector<T, 3>& vector3, const T& value) {
    x = vector3[0];
    y = vector3[1];
    z = vector3[2];
    w = value;
  }

  inline Vector(const Vector<T, 2>& v12, const Vector<T, 2>& v34) {
    x = v12[0];
    y = v12[1];
    z = v34[0];
    w = v34[1];
  }

  explicit inline Vector(const VectorPacked<T, 4>& vector) {
    MATHFU_VECTOR_OPERATION(data_[i] = vector.data[i]);
  }

  inline T& operator()(const int i) { return data_[i]; }

  inline const T& operator()(const int i) const { return data_[i]; }

  inline T& operator[](const int i) { return data_[i]; }

  inline const T& operator[](const int i) const { return data_[i]; }

  inline Vector<T, 3> xyz() { return Vector<T, 3>(x, y, z); }

  inline const Vector<T, 3> xyz() const { return Vector<T, 3>(x, y, z); }

  inline Vector<T, 2> xy() { return Vector<T, 2>(x, y); }

  inline const Vector<T, 2> xy() const { return Vector<T, 2>(x, y); }

  inline Vector<T, 2> zw() { return Vector<T, 2>(z, w); }

  inline const Vector<T, 2> zw() const { return Vector<T, 2>(z, w); }

  inline void Pack(VectorPacked<T, 4>* const vector) const {
    MATHFU_VECTOR_OPERATION(vector->data[i] = data_[i]);
  }

  inline T LengthSquared() const { return LengthSquaredHelper(*this); }

  inline T Length() const { return LengthHelper(*this); }

  inline T Normalize() { return NormalizeHelper(*this); }

  inline Vector<T, 4> Normalized() const { return NormalizedHelper(*this); }

  template <typename CompatibleT>
  static inline Vector<T, 4> FromType(const CompatibleT& compatible) {
    return FromTypeHelper<T, d, CompatibleT>(compatible);
  }

  template <typename CompatibleT>
  static inline CompatibleT ToType(const Vector<T, 4>& v) {
    return ToTypeHelper<T, d, CompatibleT>(v);
  }

  static inline T DotProduct(const Vector<T, 4>& v1, const Vector<T, 4>& v2) {
    return DotProductHelper(v1, v2);
  }

  static inline Vector<T, 4> HadamardProduct(const Vector<T, 4>& v1,
                                             const Vector<T, 4>& v2) {
    return HadamardProductHelper(v1, v2);
  }

  static inline Vector<T, 4> Lerp(const Vector<T, 4>& v1,
                                  const Vector<T, 4>& v2, const T percent) {
    return LerpHelper(v1, v2, percent);
  }

  static inline Vector<T, 4> RandomInRange(const Vector<T, 4>& min,
                                           const Vector<T, 4>& max) {
    return RandomInRangeHelper(min, max);
  }

  static inline Vector<T, 4> Max(const Vector<T, 4>& v1,
                                 const Vector<T, 4>& v2) {
    return MaxHelper(v1, v2);
  }

  static inline Vector<T, 4> Min(const Vector<T, 4>& v1,
                                 const Vector<T, 4>& v2) {
    return MinHelper(v1, v2);
  }

  MATHFU_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

#include "mathfu/internal/disable_warnings_begin.h"
  union {
    T data_[4];
    struct {
      T x;
      T y;
      T z;
      T w;
    };
  };
#include "mathfu/internal/disable_warnings_end.h"
};

template <class T>
struct VectorPacked<T, 4> {
  /// Create an uninitialized VectorPacked.
  VectorPacked() {}

  /// Create a VectorPacked from a Vector.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to create the VectorPacked from.
  explicit VectorPacked(const Vector<T, 4>& vector) { vector.Pack(this); }

  /// Copy a Vector to a VectorPacked.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to copy to the VectorPacked.
  /// @returns A reference to this VectorPacked.
  VectorPacked& operator=(const Vector<T, 4>& vector) {
    vector.Pack(this);
    return *this;
  }

#include "mathfu/internal/disable_warnings_begin.h"
  /// Elements of the packed vector one per dimension.
  union {
    T data[4];
    struct {
      T x;
      T y;
      T z;
      T w;
    };
  };
#include "mathfu/internal/disable_warnings_end.h"
};

}  // namespace mathfu

#endif  // MATHFU_INTERNAL_VECTOR_4_H_
