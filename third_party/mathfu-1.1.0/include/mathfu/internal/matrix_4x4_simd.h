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
#ifndef MATHFU_MATRIX_4X4_SIMD_H_
#define MATHFU_MATRIX_4X4_SIMD_H_

#include "mathfu/matrix.h"

#ifdef MATHFU_COMPILE_WITH_SIMD
#include "vectorial/simd4x4f.h"
#endif

/// @file mathfu/internal/matrix_4x4_simd.h MathFu Matrix<T, 4, 4>
///       Specialization
/// @brief 4x4 specialization of mathfu::Matrix for SIMD optimized builds.
/// @see mathfu::Matrix

namespace mathfu {

#ifdef MATHFU_COMPILE_WITH_SIMD

static const Vector<float, 4> kAffineWColumn(0.0f, 0.0f, 0.0f, 1.0f);

/// @cond MATHFU_INTERNAL
template <>
class Matrix<float, 4> {
 public:
  Matrix<float, 4>() {}

  inline Matrix<float, 4>(const Matrix<float, 4>& m) {
    data_.simd_matrix.x = m.data_.simd_matrix.x;
    data_.simd_matrix.y = m.data_.simd_matrix.y;
    data_.simd_matrix.z = m.data_.simd_matrix.z;
    data_.simd_matrix.w = m.data_.simd_matrix.w;
  }

  explicit inline Matrix<float, 4>(const float& s) {
    simd4f v = simd4f_create(s, s, s, s);
    data_.simd_matrix = simd4x4f_create(v, v, v, v);
  }

  inline Matrix<float, 4>(const float& s00, const float& s10, const float& s20,
                          const float& s30, const float& s01, const float& s11,
                          const float& s21, const float& s31, const float& s02,
                          const float& s12, const float& s22, const float& s32,
                          const float& s03, const float& s13, const float& s23,
                          const float& s33) {
    data_.simd_matrix = simd4x4f_create(
        simd4f_create(s00, s10, s20, s30), simd4f_create(s01, s11, s21, s31),
        simd4f_create(s02, s12, s22, s32), simd4f_create(s03, s13, s23, s33));
  }

  explicit inline Matrix<float, 4>(const float* m) {
    data_.simd_matrix =
        simd4x4f_create(simd4f_create(m[0], m[1], m[2], m[3]),
                        simd4f_create(m[4], m[5], m[6], m[7]),
                        simd4f_create(m[8], m[9], m[10], m[11]),
                        simd4f_create(m[12], m[13], m[14], m[15]));
  }

  inline Matrix<float, 4>(const Vector<float, 4>& column0,
                          const Vector<float, 4>& column1,
                          const Vector<float, 4>& column2,
                          const Vector<float, 4>& column3) {
#if defined(MATHFU_COMPILE_WITH_PADDING)
    data_.simd_matrix = simd4x4f_create(column0.data_.simd, column1.data_.simd,
                                        column2.data_.simd, column3.data_.simd);
#else
    data_.simd_matrix = simd4x4f_create(
        simd4f_create(column0[0], column0[1], column0[2], column0[3]),
        simd4f_create(column1[0], column1[1], column1[2], column1[3]),
        simd4f_create(column2[0], column2[1], column2[2], column2[3]),
        simd4f_create(column3[0], column3[1], column3[2], column3[3]));
#endif  // defined(MATHFU_COMPILE_WITH_PADDING)
  }

  explicit inline Matrix(const VectorPacked<float, 4>* const vectors) {
    data_.simd_matrix.x = simd4f_uload4(vectors[0].data);
    data_.simd_matrix.y = simd4f_uload4(vectors[1].data);
    data_.simd_matrix.z = simd4f_uload4(vectors[2].data);
    data_.simd_matrix.w = simd4f_uload4(vectors[3].data);
  }

  inline const float& operator()(const int i, const int j) const {
    return FindElem(i, FindColumn(j));
  }

  inline float& operator()(const int i, const int j) {
    return FindElem(i, FindColumn(j));
  }

  inline const float& operator()(const int i) const {
    return this->operator[](i);
  }

  inline float& operator()(const int i) { return this->operator[](i); }

  inline const float& operator[](const int i) const {
    const int col = i / 4;
    const int row = i % 4;
    return FindElem(row, FindColumn(col));
  }

  inline float& operator[](const int i) {
    const int col = i / 4;
    const int row = i % 4;
    return FindElem(row, FindColumn(col));
  }

  inline void Pack(VectorPacked<float, 4>* const vector) const {
    simd4f_ustore4(data_.simd_matrix.x, vector[0].data);
    simd4f_ustore4(data_.simd_matrix.y, vector[1].data);
    simd4f_ustore4(data_.simd_matrix.z, vector[2].data);
    simd4f_ustore4(data_.simd_matrix.w, vector[3].data);
  }

  inline Matrix<float, 4> operator-() const {
    Matrix<float, 4> m(0.f);
    simd4x4f_sub(&m.data_.simd_matrix, &data_.simd_matrix,
                 &m.data_.simd_matrix);
    return m;
  }

  inline Matrix<float, 4> operator+(const Matrix<float, 4>& m) const {
    Matrix<float, 4> return_m;
    simd4x4f_add(&data_.simd_matrix, &m.data_.simd_matrix,
                 &return_m.data_.simd_matrix);
    return return_m;
  }

  inline Matrix<float, 4> operator-(const Matrix<float, 4>& m) const {
    Matrix<float, 4> return_m;
    simd4x4f_sub(&data_.simd_matrix, &m.data_.simd_matrix,
                 &return_m.data_.simd_matrix);
    return return_m;
  }

  inline Matrix<float, 4> operator*(const float& s) const {
    Matrix<float, 4> m(s);
    simd4x4f_mul(&m.data_.simd_matrix, &data_.simd_matrix,
                 &m.data_.simd_matrix);
    return m;
  }

  inline Matrix<float, 4> operator/(const float& s) const {
    Matrix<float, 4> m(1 / s);
    simd4x4f_mul(&m.data_.simd_matrix, &data_.simd_matrix,
                 &m.data_.simd_matrix);
    return m;
  }

  inline Vector<float, 3> operator*(const Vector<float, 3>& v) const {
    Vector<float, 3> return_v;
    Simd4fUnion temp_v;
#ifdef MATHFU_COMPILE_WITH_PADDING
    temp_v.simd = v.data_.simd;
    temp_v.float_array[3] = 1;
    simd4x4f_matrix_vector_mul(&data_.simd_matrix, &temp_v.simd,
                               &return_v.data_.simd);
    return_v *= (1 / return_v.data_.float_array[3]);
#else
    temp_v.simd = simd4f_create(v[0], v[1], v[2], 1.0f);
    simd4x4f_matrix_vector_mul(&data_.simd_matrix, &temp_v.simd, &temp_v.simd);
    simd4f_mul(temp_v.simd, simd4f_splat(temp_v.float_array[3]));
    MATHFU_VECTOR3_STORE3(temp_v.simd, return_v.data_);
#endif  // MATHFU_COMPILE_WITH_PADDING
    return return_v;
  }

  inline Vector<float, 4> operator*(const Vector<float, 4>& v) const {
    Vector<float, 4> return_v;
    simd4x4f_matrix_vector_mul(&data_.simd_matrix, &v.data_.simd,
                               &return_v.data_.simd);
    return return_v;
  }

  inline Vector<float, 4> VecMatTimes(const Vector<float, 4>& v) const {
    return Vector<float, 4>(
        simd4f_dot3_scalar(v.data_.simd, data_.simd_matrix.x),
        simd4f_dot3_scalar(v.data_.simd, data_.simd_matrix.y),
        simd4f_dot3_scalar(v.data_.simd, data_.simd_matrix.z),
        simd4f_dot3_scalar(v.data_.simd, data_.simd_matrix.w));
  }

  inline Matrix<float, 4> operator*(const Matrix<float, 4>& m) const {
    Matrix<float, 4> return_m;
    simd4x4f_matrix_mul(&data_.simd_matrix, &m.data_.simd_matrix,
                        &return_m.data_.simd_matrix);
    return return_m;
  }

  inline Matrix<float, 4> Inverse() const {
    Matrix<float, 4> return_m;
    simd4x4f_inverse(&data_.simd_matrix, &return_m.data_.simd_matrix);
    return return_m;
  }

  inline bool InverseWithDeterminantCheck(
      Matrix<float, 4, 4>* const inverse) const {
    return fabs(simd4f_get_x(simd4x4f_inverse(&data_.simd_matrix,
                                              &inverse->data_.simd_matrix))) >=
           Constants<float>::GetDeterminantThreshold();
  }

  /// Calculate the transpose of matrix.
  /// @return The transpose of the specified matrix.
  inline Matrix<float, 4, 4> Transpose() const {
    Matrix<float, 4, 4> transpose;
    simd4x4f_transpose(&data_.simd_matrix, &transpose.data_.simd_matrix);
    return transpose;
  }

  inline Vector<float, 3> TranslationVector3D() const {
    Vector<float, 3> return_v;
    MATHFU_VECTOR3_STORE3(FindColumn(3).simd, return_v.data_);
    return return_v;
  }

  inline Matrix<float, 4>& operator+=(const Matrix<float, 4>& m) {
    simd4x4f_add(&data_.simd_matrix, &m.data_.simd_matrix, &data_.simd_matrix);
    return *this;
  }

  inline Matrix<float, 4>& operator-=(const Matrix<float, 4>& m) {
    simd4x4f_sub(&data_.simd_matrix, &m.data_.simd_matrix, &data_.simd_matrix);
    return *this;
  }

  inline Matrix<float, 4>& operator*=(const float& s) {
    Matrix<float, 4> m(s);
    simd4x4f_mul(&m.data_.simd_matrix, &data_.simd_matrix, &data_.simd_matrix);
    return *this;
  }

  inline Matrix<float, 4>& operator/=(const float& s) {
    Matrix<float, 4> m(1 / s);
    simd4x4f_mul(&m.data_.simd_matrix, &data_.simd_matrix, &data_.simd_matrix);
    return *this;
  }

  inline Matrix<float, 4> operator*=(const Matrix<float, 4>& m) {
    Matrix<float, 4> copy_of_this(*this);
    simd4x4f_matrix_mul(&copy_of_this.data_.simd_matrix, &m.data_.simd_matrix,
                        &data_.simd_matrix);
    return *this;
  }

  template <typename CompatibleT>
  static inline Matrix<float, 4> FromType(const CompatibleT& compatible) {
    return FromTypeHelper<float, 4, 4, CompatibleT>(compatible);
  }

  template <typename CompatibleT>
  static inline CompatibleT ToType(const Matrix<float, 4>& m) {
    return ToTypeHelper<float, 4, 4, CompatibleT>(m);
  }

  static inline Matrix<float, 4> OuterProduct(const Vector<float, 4>& v1,
                                              const Vector<float, 4>& v2) {
    Matrix<float, 4> m;
    m.data_.simd_matrix =
        simd4x4f_create(simd4f_mul(v1.data_.simd, simd4f_splat(v2[0])),
                        simd4f_mul(v1.data_.simd, simd4f_splat(v2[1])),
                        simd4f_mul(v1.data_.simd, simd4f_splat(v2[2])),
                        simd4f_mul(v1.data_.simd, simd4f_splat(v2[3])));
    return m;
  }

  static inline Matrix<float, 4> HadamardProduct(const Matrix<float, 4>& m1,
                                                 const Matrix<float, 4>& m2) {
    Matrix<float, 4> return_m;
    simd4x4f_mul(&m1.data_.simd_matrix, &m2.data_.simd_matrix,
                 &return_m.data_.simd_matrix);
    return return_m;
  }

  static inline Matrix<float, 4> Identity() {
    Matrix<float, 4> return_m;
    simd4x4f_identity(&return_m.data_.simd_matrix);
    return return_m;
  }

  static inline Matrix<float, 4> FromTranslationVector(
      const Vector<float, 3>& v) {
    return Matrix<float, 4>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, v[0], v[1],
                            v[2], 1);
  }

  static inline Matrix<float, 4> FromScaleVector(const Vector<float, 3>& v) {
    return Matrix<float, 4>(v[0], 0, 0, 0, 0, v[1], 0, 0, 0, 0, v[2], 0, 0, 0,
                            0, 1);
  }

  static inline Matrix<float, 4> FromRotationMatrix(const Matrix<float, 3>& m) {
    return Matrix<float, 4>(m[0], m[1], m[2], 0, m[3], m[4], m[5], 0, m[6],
                            m[7], m[8], 0, 0, 0, 0, 1);
  }

  /// @brief Constructs a Matrix<float, 4> from an AffineTransform.
  ///
  /// @param affine An AffineTransform reference to be used to construct
  /// a Matrix<float, 4> by adding in the 'w' row of [0, 0, 0, 1].
  static inline Matrix<float, 4> FromAffineTransform(
      const AffineTransform& affine) {
    Matrix<float, 4> m;
    m.data_.simd_matrix.x = simd4f_uload4(&affine[0]);
    m.data_.simd_matrix.y = simd4f_uload4(&affine[4]);
    m.data_.simd_matrix.z = simd4f_uload4(&affine[8]);
    m.data_.simd_matrix.w = simd4f_uload4(&kAffineWColumn[0]);
    return m.Transpose();
  }

  /// @brief Converts a Matrix<float, 4> into an AffineTransform.
  ///
  /// @param m A Matrix<float, 4> reference to be converted into an
  /// AffineTransform by dropping the fixed 'w' row.
  ///
  /// @return Returns an AffineTransform that contains the essential
  /// transformation data from the Matrix<float, 4>.
  static inline AffineTransform ToAffineTransform(const Matrix<float, 4>& m) {
    AffineTransform affine;
    const Matrix<float, 4> mt = m.Transpose();
    simd4f_ustore4(mt.data_.simd_matrix.x, &affine[0]);
    simd4f_ustore4(mt.data_.simd_matrix.y, &affine[4]);
    simd4f_ustore4(mt.data_.simd_matrix.z, &affine[8]);
    return affine;
  }

  /// Create a 4x4 perpective matrix.
  /// @handedness: 1.0f for RH, -1.0f for LH
  static inline Matrix<float, 4, 4> Perspective(float fovy, float aspect,
                                                float znear, float zfar,
                                                float handedness = 1.0f) {
    return PerspectiveHelper(fovy, aspect, znear, zfar, handedness);
  }

  /// Create a 4x4 orthographic matrix.
  /// @param handedness 1.0f for RH, -1.0f for LH
  static inline Matrix<float, 4, 4> Ortho(float left, float right, float bottom,
                                          float top, float znear, float zfar,
                                          float handedness = 1.0f) {
    return OrthoHelper(left, right, bottom, top, znear, zfar, handedness);
  }

  /// Create a 3-dimensional camera matrix.
  /// @param at The look-at target of the camera.
  /// @param eye The position of the camera.
  /// @param up The up vector in the world, for example (0, 1, 0) if the
  /// @handedness: 1.0f for RH, -1.0f for LH
  /// TODO: Change default handedness to 1.0f, to match Perspective().
  /// y-axis is up.
  static inline Matrix<float, 4, 4> LookAt(const Vector<float, 3>& at,
                                           const Vector<float, 3>& eye,
                                           const Vector<float, 3>& up,
                                           float handedness = -1.0f) {
    return LookAtHelper(at, eye, up, handedness);
  }

  /// @brief Get the 3D position in object space from a window coordinate.
  ///
  /// @param window_coord The window coordinate. The z value is for depth.
  /// A window coordinate on the near plane will have 0 as the z value.
  /// And a window coordinate on the far plane will have 1 as the z value.
  /// z value should be with in [0, 1] here.
  /// @param model_view The Model View matrix.
  /// @param projection The projection matrix.
  /// @param window_width Width of the window.
  /// @param window_height Height of the window.
  /// @return the mapped 3D position in object space.
  static inline Vector<float, 3> UnProject(
      const Vector<float, 3>& window_coord,
      const Matrix<float, 4, 4>& model_view,
      const Matrix<float, 4, 4>& projection, const float window_width,
      const float window_height) {
    Vector<float, 3> result;
    UnProjectHelper(window_coord, model_view, projection, window_width,
                    window_height, result);
    return result;
  }

  // Dimensions of the matrix.
  /// Number of rows in the matrix.
  static const int kRows = 4;
  /// Number of columns in the matrix.
  static const int kColumns = 4;
  /// Total number of elements in the matrix.
  static const int kElements = 4 * 4;

  MATHFU_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

 private:
  inline const Simd4fUnion& FindColumn(const int i) const {
    return data_.simd4f_union_array[i];
  }

  inline Simd4fUnion& FindColumn(const int i) {
    return data_.simd4f_union_array[i];
  }

  static inline const float& FindElem(const int i, const Simd4fUnion& column) {
    return column.float_array[i];
  }

  static inline float& FindElem(const int i, Simd4fUnion& column) {
    return column.float_array[i];
  }

  // Contents of the Matrix in different representations to work around
  // strict aliasing rules.
  union {
    simd4x4f simd_matrix;
    Simd4fUnion simd4f_union_array[4];
    float float_array[16];
  } data_;
};

inline Matrix<float, 4> operator*(const float& s, const Matrix<float, 4>& m) {
  return m * s;
}

inline Vector<float, 4> operator*(const Vector<float, 4>& v,
                                  const Matrix<float, 4>& m) {
  return m.VecMatTimes(v);
}
/// @endcond
#endif  // MATHFU_COMPILE_WITH_SIMD
}  // namespace mathfu

#endif  // MATHFU_MATRIX_4X4_SIMD_H_
