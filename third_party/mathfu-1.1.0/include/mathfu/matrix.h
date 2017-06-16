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
#ifndef MATHFU_MATRIX_H_
#define MATHFU_MATRIX_H_

#include "mathfu/utilities.h"
#include "mathfu/vector.h"

#include <cmath>

#include <assert.h>

/// @file mathfu/matrix.h
/// @brief Matrix class and functions.
/// @addtogroup mathfu_matrix
///
/// MathFu provides a generic Matrix implementation which is specialized
/// for 4x4 matrices to take advantage of optimization opportunities using
/// SIMD instructions.

#ifdef _MSC_VER
#pragma warning(push)
// The following disables warnings for MATHFU_MAT_OPERATION.
// The buffer overrun warning must be disabled as MSVC doesn't treat
// "columns" as constant and therefore assumes that it's possible
// to overrun arrays indexed by "i".
// The conditional expression is constant warning is disabled since
// MSVC decides that "columns" *is* constant when unrolling the operation
// loop.
#pragma warning(disable : 4127)  // conditional expression is constant
#pragma warning(disable : 4789)  // buffer overrun
#if _MSC_VER >= 1900             // MSVC 2015
#pragma warning(disable : 4456)  // allow shadowing in unrolled loops
#pragma warning(disable : 4723)  // suppress "potential divide by 0" warning
#endif                           // _MSC_VER >= 1900
#endif                           // _MSC_VER

/// @cond MATHFU_INTERNAL
/// The stride of a vector (e.g Vector<T, 3>) when cast as an array of floats.
#define MATHFU_VECTOR_STRIDE_FLOATS(vector) (sizeof(vector) / sizeof(float))
/// @endcond

/// @cond MATHFU_INTERNAL
/// This will unroll loops for matrices with <= 4 columns
#define MATHFU_MAT_OPERATION(OP) MATHFU_UNROLLED_LOOP(i, columns, OP)
/// @endcond

/// @cond MATHFU_INTERNAL
/// This will perform a given OP on each matrix column and return the result
#define MATHFU_MAT_OPERATOR(OP)                   \
  {                                               \
    Matrix<T, rows, columns> result;              \
    MATHFU_MAT_OPERATION(result.data_[i] = (OP)); \
    return result;                                \
  }
/// @endcond

/// @cond MATHFU_INTERNAL
/// This will perform a given OP on each matrix column
#define MATHFU_MAT_SELF_OPERATOR(OP) \
  {                                  \
    MATHFU_MAT_OPERATION(OP);        \
    return *this;                    \
  }
/// @endcond

/// @cond MATHFU_INTERNAL
/// This macro will take the dot product for a row from data1 and a column from
/// data2.
#define MATHFU_MATRIX_4X4_DOT(data1, data2, r)               \
  ((data1)[r] * (data2)[0] + (data1)[(r) + 4] * (data2)[1] + \
   (data1)[(r) + 8] * (data2)[2] + (data1)[(r) + 12] * (data2)[3])
/// @endcond

/// @cond MATHFU_INTERNAL
#define MATHFU_MATRIX_3X3_DOT(data1, data2, r, size)              \
  ((data1)[r] * (data2)[0] + (data1)[(r) + (size)] * (data2)[1] + \
   (data1)[(r) + 2 * (size)] * (data2)[2])
/// @endcond

namespace mathfu {

/// @cond MATHFU_INTERNAL
template <class T, int rows, int columns = rows>
class Matrix;
template <class T, int rows, int columns>
inline Matrix<T, rows, columns> IdentityHelper();
template <bool check_invertible, class T, int rows, int columns>
inline bool InverseHelper(const Matrix<T, rows, columns>& m,
                          Matrix<T, rows, columns>* const inverse);
template <class T, int rows, int columns>
inline void TimesHelper(const Matrix<T, rows, columns>& m1,
                        const Matrix<T, rows, columns>& m2,
                        Matrix<T, rows, columns>* out_m);
template <class T, int rows, int columns>
static inline Matrix<T, rows, columns> OuterProductHelper(
    const Vector<T, rows>& v1, const Vector<T, columns>& v2);
template <class T>
inline Matrix<T, 4, 4> PerspectiveHelper(T fovy, T aspect, T znear, T zfar,
                                         T handedness);
template <class T>
static inline Matrix<T, 4, 4> OrthoHelper(T left, T right, T bottom, T top,
                                          T znear, T zfar, T handedness);
template <class T>
static inline Matrix<T, 4, 4> LookAtHelper(const Vector<T, 3>& at,
                                           const Vector<T, 3>& eye,
                                           const Vector<T, 3>& up,
                                           T handedness);
template <class T>
static inline bool UnProjectHelper(const Vector<T, 3>& window_coord,
                                   const Matrix<T, 4, 4>& model_view,
                                   const Matrix<T, 4, 4>& projection,
                                   const float window_width,
                                   const float window_height,
                                   Vector<T, 3>& result);

template <typename T, int rows, int columns, typename CompatibleT>
static inline Matrix<T, rows, columns> FromTypeHelper(const CompatibleT& compatible);

template <typename T, int rows, int columns, typename CompatibleT>
static inline CompatibleT ToTypeHelper(const Matrix<T, rows, columns>& m);
/// @endcond

/// @addtogroup mathfu_matrix
/// @{
/// @class Matrix
/// @brief Matrix stores a set of "rows" by "columns" elements of type T
/// and provides functions that operate on the set of elements.
///
/// @tparam T type of each element in the matrix.
/// @tparam rows Number of rows in the matrix.
/// @tparam columns Number of columns in the matrix.
template <class T, int rows, int columns>
class Matrix {
 public:
  /// @brief Construct a Matrix of uninitialized values.
  inline Matrix() {}

  /// @brief Construct a Matrix from another Matrix copying each element.
  ////
  /// @param m Matrix that the data will be copied from.
  inline Matrix(const Matrix<T, rows, columns>& m) {
    MATHFU_MAT_OPERATION(data_[i] = m.data_[i]);
  }

  /// @brief Construct a Matrix from a single float.
  ///
  /// @param s Scalar value used to initialize each element of the matrix.
  explicit inline Matrix(const T& s) {
    MATHFU_MAT_OPERATION((data_[i] = Vector<T, rows>(s)));
  }

  /// @brief Construct a Matrix from four floats.
  ///
  /// @note This method only works with a 2x2 Matrix.
  ///
  /// @param s00 Value of the first row and column.
  /// @param s10 Value of the second row, first column.
  /// @param s01 Value of the first row, second column.
  /// @param s11 Value of the second row and column.
  inline Matrix(const T& s00, const T& s10, const T& s01, const T& s11) {
    MATHFU_STATIC_ASSERT(rows == 2 && columns == 2);
    data_[0] = Vector<T, rows>(s00, s10);
    data_[1] = Vector<T, rows>(s01, s11);
  }

  /// @brief Create a Matrix from nine floats.
  ///
  /// @note This method only works with a 3x3 Matrix.
  ///
  /// @param s00 Value of the first row and column.
  /// @param s10 Value of the second row, first column.
  /// @param s20 Value of the third row, first column.
  /// @param s01 Value of the first row, second column.
  /// @param s11 Value of the second row and column.
  /// @param s21 Value of the third row, second column.
  /// @param s02 Value of the first row, third column.
  /// @param s12 Value of the second row, third column.
  /// @param s22 Value of the third row and column.
  inline Matrix(const T& s00, const T& s10, const T& s20, const T& s01,
                const T& s11, const T& s21, const T& s02, const T& s12,
                const T& s22) {
    MATHFU_STATIC_ASSERT(rows == 3 && columns == 3);
    data_[0] = Vector<T, rows>(s00, s10, s20);
    data_[1] = Vector<T, rows>(s01, s11, s21);
    data_[2] = Vector<T, rows>(s02, s12, s22);
  }

  /// @brief Creates a Matrix from twelve floats.
  ///
  /// @note This method only works with Matrix<float, 4, 3>.
  ///
  ///
  /// @param s00 Value of the first row and column.
  /// @param s10 Value of the second row, first column.
  /// @param s20 Value of the third row, first column.
  /// @param s30 Value of the fourth row, first column.
  /// @param s01 Value of the first row, second column.
  /// @param s11 Value of the second row and column.
  /// @param s21 Value of the third row, second column.
  /// @param s31 Value of the fourth row, second column.
  /// @param s02 Value of the first row, third column.
  /// @param s12 Value of the second row, third column.
  /// @param s22 Value of the third row and column.
  /// @param s32 Value of the fourth row, third column.
  inline Matrix(const T& s00, const T& s10, const T& s20, const T& s30,
                const T& s01, const T& s11, const T& s21, const T& s31,
                const T& s02, const T& s12, const T& s22, const T& s32) {
    MATHFU_STATIC_ASSERT(rows == 4 && columns == 3);
    data_[0] = Vector<T, rows>(s00, s10, s20, s30);
    data_[1] = Vector<T, rows>(s01, s11, s21, s31);
    data_[2] = Vector<T, rows>(s02, s12, s22, s32);
  }

  /// @brief Create a Matrix from sixteen floats.
  ///
  /// @note This method only works with a 4x4 Matrix.
  ///
  /// @param s00 Value of the first row and column.
  /// @param s10 Value of the second row, first column.
  /// @param s20 Value of the third row, first column.
  /// @param s30 Value of the fourth row, first column.
  /// @param s01 Value of the first row, second column.
  /// @param s11 Value of the second row and column.
  /// @param s21 Value of the third row, second column.
  /// @param s31 Value of the fourth row, second column.
  /// @param s02 Value of the first row, third column.
  /// @param s12 Value of the second row, third column.
  /// @param s22 Value of the third row and column.
  /// @param s32 Value of the fourth row, third column.
  /// @param s03 Value of the first row, fourth column.
  /// @param s13 Value of the second row, fourth column.
  /// @param s23 Value of the third row, fourth column.
  /// @param s33 Value of the fourth row and column.
  inline Matrix(const T& s00, const T& s10, const T& s20, const T& s30,
                const T& s01, const T& s11, const T& s21, const T& s31,
                const T& s02, const T& s12, const T& s22, const T& s32,
                const T& s03, const T& s13, const T& s23, const T& s33) {
    MATHFU_STATIC_ASSERT(rows == 4 && columns == 4);
    data_[0] = Vector<T, rows>(s00, s10, s20, s30);
    data_[1] = Vector<T, rows>(s01, s11, s21, s31);
    data_[2] = Vector<T, rows>(s02, s12, s22, s32);
    data_[3] = Vector<T, rows>(s03, s13, s23, s33);
  }

  /// @brief Create 4x4 Matrix from 4, 4 element vectors.
  ///
  /// @note This method only works with a 4x4 Matrix.
  ///
  /// @param column0 Vector used for the first column.
  /// @param column1 Vector used for the second column.
  /// @param column2 Vector used for the third column.
  /// @param column3 Vector used for the fourth column.
  inline Matrix(const Vector<T, 4>& column0, const Vector<T, 4>& column1,
                const Vector<T, 4>& column2, const Vector<T, 4>& column3) {
    MATHFU_STATIC_ASSERT(rows == 4 && columns == 4);
    data_[0] = column0;
    data_[1] = column1;
    data_[2] = column2;
    data_[3] = column3;
  }

  /// @brief Create a Matrix from the first row * column elements of an array.
  ///
  /// @param a Array of values that the matrix will be iniitlized to.
  explicit inline Matrix(const T* const a) {
    MATHFU_MAT_OPERATION((data_[i] = Vector<T, rows>(&a[i * columns])));
  }

  /// @brief Create a Matrix from an array of "columns", "rows" element packed
  /// vectors.
  ///
  /// @param vectors Array of "columns", "rows" element packed vectors.
  explicit inline Matrix(const VectorPacked<T, rows>* const vectors) {
    MATHFU_MAT_OPERATION((data_[i] = Vector<T, rows>(vectors[i])));
  }

  /// @brief Access an element of the matrix.
  ///
  /// @param row Index of the row to access.
  /// @param column Index of the column to access.
  /// @return Const reference to the element.
  inline const T& operator()(const int row, const int column) const {
    return data_[column][row];
  }

  /// @brief Access an element of the Matrix.
  ///
  /// @param row Index of the row to access.
  /// @param column Index of the column to access.
  /// @return Reference to the data that can be modified by the caller.
  inline T& operator()(const int row, const int column) {
    return data_[column][row];
  }

  /// @brief Access an element of the Matrix.
  ///
  /// @param i Index of the element to access in flattened memory.  Where
  /// the column accessed is i / rows and the row is i % rows.
  /// @return Reference to the data that can be modified by the caller.
  inline const T& operator()(const int i) const { return operator[](i); }

  /// @brief Access an element of the Matrix.
  ///
  /// @param i Index of the element to access in flattened memory.  Where
  /// the column accessed is i / rows and the row is i % rows.
  /// @return Reference to the data that can be modified by the caller.
  inline T& operator()(const int i) { return operator[](i); }

  /// @brief Access an element of the Matrix.
  ///
  /// @param i Index of the element to access in flattened memory.  Where
  /// the column accessed is i / rows and the row is i % rows.
  /// @return Const reference to the data.
  inline const T& operator[](const int i) const {
    return const_cast<Matrix<T, rows, columns>*>(this)->operator[](i);
  }

  /// @brief Access an element of the Matrix.
  ///
  /// @param i Index of the element to access in flattened memory.  Where
  /// the column accessed is i / rows and the row is i % rows.
  /// @return Reference to the data that can be modified by the caller.
  inline T& operator[](const int i) {
#if defined(MATHFU_COMPILE_WITH_PADDING)
    // In this case Vector<T, 3> is padded, so the element offset must be
    // accessed using the array operator.
    if (rows == 3) {
      const int row = i % rows;
      const int col = i / rows;
      return data_[col][row];
    } else {
      return reinterpret_cast<T*>(data_)[i];
    }
#else
    return reinterpret_cast<T*>(data_)[i];
#endif  // defined(MATHFU_COMPILE_WITH_PADDING)
  }

  /// @brief Pack the matrix to an array of "rows" element vectors,
  /// one vector per matrix column.
  ///
  /// @param vector Array of "columns" entries to write to.
  inline void Pack(VectorPacked<T, rows>* const vector) const {
    MATHFU_MAT_OPERATION(GetColumn(i).Pack(&vector[i]));
  }

  /// @cond MATHFU_INTERNAL
  /// @brief Access a column vector of the Matrix.
  ///
  /// @param i Index of the column to access.
  /// @return Reference to the data that can be modified by the caller.
  inline Vector<T, rows>& GetColumn(const int i) { return data_[i]; }

  /// @brief Access a column vector of the Matrix.
  ///
  /// @param i Index of the column to access.
  /// @return Const reference to the data.
  inline const Vector<T, rows>& GetColumn(const int i) const {
    return data_[i];
  }
  /// @endcond

  /// @brief Negate this Matrix.
  ///
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator-() const {
    MATHFU_MAT_OPERATOR(-data_[i]);
  }

  /// @brief Add a Matrix to this Matrix.
  ///
  /// @param m Matrix to add to this Matrix.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator+(
      const Matrix<T, rows, columns>& m) const {
    MATHFU_MAT_OPERATOR(data_[i] + m.data_[i]);
  }

  /// @brief Subtract a Matrix from this Matrix.
  ///
  /// @param m Matrix to subtract from this Matrix.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator-(
      const Matrix<T, rows, columns>& m) const {
    MATHFU_MAT_OPERATOR(data_[i] - m.data_[i]);
  }

  /// @brief Add a scalar to each element of this Matrix.
  ///
  /// @param s Scalar to add to this Matrix.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator+(const T& s) const {
    MATHFU_MAT_OPERATOR(data_[i] + s);
  }

  /// @brief Subtract a scalar from each element of this Matrix.
  ///
  /// @param s Scalar to subtract from this matrix.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator-(const T& s) const {
    MATHFU_MAT_OPERATOR(data_[i] - s);
  }

  /// @brief Multiply each element of this Matrix with a scalar.
  ///
  /// @param s Scalar to multiply with this Matrix.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator*(const T& s) const {
    MATHFU_MAT_OPERATOR(data_[i] * s);
  }

  /// @brief Divide each element of this Matrix with a scalar.
  ///
  /// @param s Scalar to divide this Matrix with.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator/(const T& s) const {
    return (*this) * (1 / s);
  }

  /// @brief Multiply this Matrix with another Matrix.
  ///
  /// @param m Matrix to multiply with this Matrix.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> operator*(
      const Matrix<T, rows, columns>& m) const {
    Matrix<T, rows, columns> result;
    TimesHelper(*this, m, &result);
    return result;
  }

  /// @brief Add a Matrix to this Matrix (in-place).
  ///
  /// @param m Matrix to add to this Matrix.
  /// @return Reference to this class.
  inline Matrix<T, rows, columns>& operator+=(
      const Matrix<T, rows, columns>& m) {
    MATHFU_MAT_SELF_OPERATOR(data_[i] += m.data_[i]);
  }

  /// @brief Subtract a Matrix from this Matrix (in-place).
  ///
  /// @param m Matrix to subtract from this Matrix.
  /// @return Reference to this class.
  inline Matrix<T, rows, columns>& operator-=(
      const Matrix<T, rows, columns>& m) {
    MATHFU_MAT_SELF_OPERATOR(data_[i] -= m.data_[i]);
  }

  /// @brief Add a scalar to each element of this Matrix (in-place).
  ///
  /// @param s Scalar to add to each element of this Matrix.
  /// @return Reference to this class.
  inline Matrix<T, rows, columns>& operator+=(const T& s) {
    MATHFU_MAT_SELF_OPERATOR(data_[i] += s);
  }

  /// @brief Subtract a scalar from each element of this Matrix (in-place).
  ///
  /// @param s Scalar to subtract from each element of this Matrix.
  /// @return Reference to this class.
  inline Matrix<T, rows, columns>& operator-=(const T& s) {
    MATHFU_MAT_SELF_OPERATOR(data_[i] -= s);
  }

  /// @brief Multiply each element of this Matrix with a scalar (in-place).
  ///
  /// @param s Scalar to multiply with each element of this Matrix.
  /// @return Reference to this class.
  inline Matrix<T, rows, columns>& operator*=(const T& s) {
    MATHFU_MAT_SELF_OPERATOR(data_[i] *= s);
  }

  /// @brief Divide each element of this Matrix by a scalar (in-place).
  ///
  /// @param s Scalar to divide this Matrix by.
  /// @return Reference to this class.
  inline Matrix<T, rows, columns>& operator/=(const T& s) {
    return (*this) *= (1 / s);
  }

  /// @brief Multiply this Matrix with another Matrix (in-place).
  ///
  /// @param m Matrix to multiply with this Matrix.
  /// @return Reference to this class.
  inline Matrix<T, rows, columns>& operator*=(
      const Matrix<T, rows, columns>& m) {
    const Matrix<T, rows, columns> copy_of_this(*this);
    TimesHelper(copy_of_this, m, this);
    return *this;
  }

  /// @brief Calculate the inverse of this Matrix.
  ///
  /// This calculates the inverse Matrix such that
  /// <code>(m * m).Inverse()</code> is the identity.
  /// @return Matrix containing the result.
  inline Matrix<T, rows, columns> Inverse() const {
    Matrix<T, rows, columns> inverse;
    InverseHelper<false>(*this, &inverse);
    return inverse;
  }

  /// @brief Calculate the inverse of this Matrix.
  ///
  /// This calculates the inverse Matrix such that
  /// <code>(m * m).Inverse()</code> is the identity.
  /// By contrast to Inverse() this returns whether the matrix is invertible.
  ///
  /// The invertible check simply compares the calculated determinant with
  /// Constants<T>::GetDeterminantThreshold() to roughly determine whether the
  /// matrix is invertible.  This simple check works in common cases but will
  /// fail for corner cases where the matrix is a combination of huge and tiny
  /// values that can't be accurately represented by the floating point
  /// datatype T.  More extensive checks (relative to the input values) are
  /// possible but <b>far</b> more expensive, complicated and difficult to
  /// test.
  /// @return Whether the matrix is invertible.
  inline bool InverseWithDeterminantCheck(
      Matrix<T, rows, columns>* const inverse) const {
    return InverseHelper<true>(*this, inverse);
  }

  /// @brief Calculate the transpose of this Matrix.
  ///
  /// @return The transpose of the specified Matrix.
  inline Matrix<T, columns, rows> Transpose() const {
    Matrix<T, columns, rows> transpose;
    MATHFU_UNROLLED_LOOP(
        i, columns, MATHFU_UNROLLED_LOOP(
                        j, rows, transpose.GetColumn(j)[i] = GetColumn(i)[j]))
    return transpose;
  }

  /// @brief Get the 2-dimensional translation of a 2-dimensional affine
  /// transform.
  ///
  /// @note 2-dimensional affine transforms are represented by 3x3 matrices.
  /// @return Vector with the first two components of column 2 of this Matrix.
  inline Vector<T, 2> TranslationVector2D() const {
    MATHFU_STATIC_ASSERT(rows == 3 && columns == 3);
    return Vector<T, 2>(data_[2][0], data_[2][1]);
  }

  /// @brief Get the 3-dimensional translation of a 3-dimensional affine
  /// transform.
  ///
  /// @note 3-dimensional affine transforms are represented by 4x4 matrices.
  /// @return Vector with the first three components of column 3.
  inline Vector<T, 3> TranslationVector3D() const {
    MATHFU_STATIC_ASSERT(rows == 4 && columns == 4);
    return Vector<T, 3>(data_[3][0], data_[3][1], data_[3][2]);
  }

  /// @brief Load from any byte-wise compatible external matrix.
  ///
  /// Format should be `columns` vectors, each holding `rows` values of type T.
  ///
  /// Use this for safe conversion from external matrix classes.
  /// Often, external libraries will have their own matrix types that are,
  /// byte-for-byte, exactly the same as mathfu::Matrix. This function allows
  /// you to load a mathfu::Matrix from those external types, without potential
  /// aliasing bugs that are caused by casting.
  ///
  /// @note If your external type gives you access to a T*, then you can
  ///       equivalently use the Matrix(const T*) constructor.
  ///
  /// @param compatible reference to a byte-wise compatible matrix structure;
  ///                   array of columns x rows Ts.
  /// @returns `compatible` loaded as a mathfu::Matrix.
  template <typename CompatibleT>
  static inline Matrix<T, rows, columns> FromType(const CompatibleT& compatible) {
    return FromTypeHelper<T, rows, columns, CompatibleT>(compatible);
  }

  /// @brief Load into any byte-wise compatible external matrix.
  ///
  /// Format should be `columns` vectors, each holding `rows` values of type T.
  ///
  /// Use this for safe conversion to external matrix classes.
  /// Often, external libraries will have their own matrix types that are,
  /// byte-for-byte, exactly the same as mathfu::Matrix. This function allows
  /// you to load an external type from a mathfu::Matrix, without potential
  /// aliasing bugs that are caused by casting.
  ///
  /// @param m reference to mathfu::Matrix to convert.
  /// @returns CompatibleT loaded from m.
  template <typename CompatibleT>
  static inline CompatibleT ToType(const Matrix<T, rows, columns>& m) {
    return ToTypeHelper<T, rows, columns, CompatibleT>(m);
  }

  /// @brief Calculate the outer product of two Vectors.
  ///
  /// @return Matrix containing the result.
  static inline Matrix<T, rows, columns> OuterProduct(
      const Vector<T, rows>& v1, const Vector<T, columns>& v2) {
    return OuterProductHelper(v1, v2);
  }

  /// @brief Calculate the hadamard / component-wise product of two matrices.
  ///
  /// @param m1 First Matrix.
  /// @param m2 Second Matrix.
  /// @return Matrix containing the result.
  static inline Matrix<T, rows, columns> HadamardProduct(
      const Matrix<T, rows, columns>& m1, const Matrix<T, rows, columns>& m2) {
    MATHFU_MAT_OPERATOR(m1[i] * m2[i]);
  }

  /// @brief Calculate the identity Matrix.
  ///
  /// @return Matrix containing the result.
  static inline Matrix<T, rows, columns> Identity() {
    return IdentityHelper<T, rows, columns>();
  }

  /// @brief Create a 3x3 translation Matrix from a 2-dimensional Vector.
  ///
  /// This matrix will have an empty or zero rotation component.
  ///
  /// @param v Vector of size 2.
  /// @return Matrix containing the result.
  static inline Matrix<T, 3> FromTranslationVector(const Vector<T, 2>& v) {
    return Matrix<T, 3>(1, 0, 0, 0, 1, 0, v[0], v[1], 1);
  }

  /// @brief Create a 4x4 translation Matrix from a 3-dimensional Vector.
  ///
  /// This matrix will have an empty or zero rotation component.
  ///
  /// @param v The vector of size 3.
  /// @return Matrix containing the result.
  static inline Matrix<T, 4> FromTranslationVector(const Vector<T, 3>& v) {
    return Matrix<T, 4>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, v[0], v[1], v[2],
                        1);
  }

  /// @brief Create a square Matrix with the diagonal component set to v.
  ///
  /// This is an affine transform matrix, so the dimension of the vector is
  /// one less than the dimension of the matrix.
  ///
  /// @param v Vector containing components for scaling.
  /// @return Matrix with v along the diagonal, and 1 in the bottom right.
  static inline Matrix<T, rows> FromScaleVector(const Vector<T, rows - 1>& v) {
    // TODO OPT: Use a helper function in a similar way to Identity to
    // construct the matrix for the specialized cases 2, 3, 4, and only run
    // this method in the general case. This will also allow you to use the
    // helper methods from specialized classes like Matrix<T, 4, 4>.
    Matrix<T, rows> return_matrix(Identity());
    for (int i = 0; i < rows - 1; ++i) return_matrix(i, i) = v[i];
    return return_matrix;
  }

  /// @brief Create a 4x4 Matrix from a 3x3 rotation Matrix.
  ///
  /// This Matrix will have an empty or zero translation component.
  ///
  /// @param m 3x3 rotation Matrix.
  /// @return Matrix containing the result.
  static inline Matrix<T, 4> FromRotationMatrix(const Matrix<T, 3>& m) {
    return Matrix<T, 4>(m[0], m[1], m[2], 0, m[3], m[4], m[5], 0, m[6], m[7],
                        m[8], 0, 0, 0, 0, 1);
  }

  /// @brief Constructs a Matrix<float, 4> from an AffineTransform.
  ///
  /// @param affine An AffineTransform reference to be used to construct
  /// a Matrix<float, 4> by adding in the 'w' row of [0, 0, 0, 1].
  static inline Matrix<T, 4> FromAffineTransform(
      const Matrix<T, 4, 3>& affine) {
    return Matrix<T, 4>(affine[0], affine[4], affine[8], static_cast<T>(0),
                        affine[1], affine[5], affine[9], static_cast<T>(0),
                        affine[2], affine[6], affine[10], static_cast<T>(0),
                        affine[3], affine[7], affine[11], static_cast<T>(1));
  }

  /// @brief Converts a Matrix<float, 4> into an AffineTransform.
  ///
  /// @param m A Matrix<float, 4> reference to be converted into an
  /// AffineTransform by dropping the fixed 'w' row.
  ///
  /// @return Returns an AffineTransform that contains the essential
  /// transformation data from the Matrix<float, 4>.
  static inline Matrix<T, 4, 3> ToAffineTransform(const Matrix<T, 4>& m) {
    return Matrix<T, 4, 3>(m[0], m[4], m[8], m[12], m[1], m[5], m[9], m[13],
                           m[2], m[6], m[10], m[14]);
  }

  /// @brief Create a 3x3 rotation Matrix from a 2D normalized directional
  /// Vector around the X axis.
  ///
  /// @param v 2D normalized directional Vector.
  /// @return Matrix containing the result.
  static inline Matrix<T, 3> RotationX(const Vector<T, 2>& v) {
    return Matrix<T, 3>(1, 0, 0, 0, v.x, v.y, 0, -v.y, v.x);
  }

  /// @brief Create a 3x3 rotation Matrix from a 2D normalized directional
  /// Vector around the Y axis.
  ///
  /// @param v 2D normalized directional Vector.
  /// @return Matrix containing the result.
  static inline Matrix<T, 3> RotationY(const Vector<T, 2>& v) {
    return Matrix<T, 3>(v.x, 0, -v.y, 0, 1, 0, v.y, 0, v.x);
  }

  /// @brief Create a 3x3 rotation Matrix from a 2D normalized directional
  /// Vector around the Z axis.
  ///
  /// @param v 2D normalized directional Vector.
  /// @return Matrix containing the result.
  static inline Matrix<T, 3> RotationZ(const Vector<T, 2>& v) {
    return Matrix<T, 3>(v.x, v.y, 0, -v.y, v.x, 0, 0, 0, 1);
  }

  /// @brief Create a 3x3 rotation Matrix from an angle (in radians) around
  /// the X axis.
  ///
  /// @param angle Angle (in radians).
  /// @return Matrix containing the result.
  static inline Matrix<T, 3> RotationX(T angle) {
    return RotationX(Vector<T, 2>(cosf(angle), sinf(angle)));
  }

  /// @brief Create a 3x3 rotation Matrix from an angle (in radians) around
  /// the Y axis.
  ///
  /// @param angle Angle (in radians).
  /// @return Matrix containing the result.
  static inline Matrix<T, 3> RotationY(T angle) {
    return RotationY(Vector<T, 2>(cosf(angle), sinf(angle)));
  }

  /// @brief Create a 3x3 rotation Matrix from an angle (in radians)
  /// around the Z axis.
  ///
  /// @param angle Angle (in radians).
  /// @return Matrix containing the result.
  static inline Matrix<T, 3> RotationZ(T angle) {
    return RotationZ(Vector<T, 2>(cosf(angle), sinf(angle)));
  }

  /// @brief Create a 4x4 perspective Matrix.
  ///
  /// @param fovy Field of view.
  /// @param aspect Aspect ratio.
  /// @param znear Near plane location.
  /// @param zfar Far plane location.
  /// @param handedness 1.0f for RH, -1.0f for LH
  /// @return 4x4 perspective Matrix.
  static inline Matrix<T, 4, 4> Perspective(T fovy, T aspect, T znear, T zfar,
                                            T handedness = 1) {
    return PerspectiveHelper(fovy, aspect, znear, zfar, handedness);
  }

  /// @brief Create a 4x4 orthographic Matrix.
  ///
  /// @param left Left extent.
  /// @param right Right extent.
  /// @param bottom Bottom extent.
  /// @param top Top extent.
  /// @param znear Near plane location.
  /// @param zfar Far plane location.
  /// @param handedness 1.0f for RH, -1.0f for LH
  /// @return 4x4 orthographic Matrix.
  static inline Matrix<T, 4, 4> Ortho(T left, T right, T bottom, T top, T znear,
                                      T zfar, T handedness = 1) {
    return OrthoHelper(left, right, bottom, top, znear, zfar, handedness);
  }

  /// @brief Create a 3-dimensional camera Matrix.
  ///
  /// @param at The look-at target of the camera.
  /// @param eye The position of the camera.
  /// @param up The up vector in the world, for example (0, 1, 0) if the
  /// y-axis is up.
  /// @param handedness 1.0f for RH, -1.0f for LH.
  /// @return 3-dimensional camera Matrix.
  /// TODO: Change default handedness to +1 so that it matches Perspective().
  static inline Matrix<T, 4, 4> LookAt(const Vector<T, 3>& at,
                                       const Vector<T, 3>& eye,
                                       const Vector<T, 3>& up,
                                       T handedness = -1) {
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
  static inline Vector<T, 3> UnProject(const Vector<T, 3>& window_coord,
                                       const Matrix<T, 4, 4>& model_view,
                                       const Matrix<T, 4, 4>& projection,
                                       const float window_width,
                                       const float window_height) {
    Vector<T, 3> result;
    UnProjectHelper(window_coord, model_view, projection, window_width,
                    window_height, result);
    return result;
  }

  /// @brief Multiply a Vector by a Matrix.
  ///
  /// @param v Vector to multiply.
  /// @param m Matrix to multiply.
  /// @return Matrix containing the result.
  friend inline Vector<T, columns> operator*(
      const Vector<T, rows>& v, const Matrix<T, rows, columns>& m) {
    const int d = columns;
    MATHFU_VECTOR_OPERATOR((Vector<T, rows>::DotProduct(m.data_[i], v)));
  }

  // Dimensions of the matrix.
  /// Number of rows in the matrix.
  static const int kRows = rows;
  /// Number of columns in the matrix.
  static const int kColumns = columns;
  /// Total number of elements in the matrix.
  static const int kElements = rows * columns;

  MATHFU_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

 private:
  Vector<T, rows> data_[columns];
};
/// @}

/// @addtogroup mathfu_matrix
/// @{

/// @brief Multiply each element of a Matrix by a scalar.
///
/// @param s Scalar to multiply by.
/// @param m Matrix to multiply.
/// @return Matrix containing the result.
/// @tparam T Type of each element in the Matrix and the scalar type.
/// @tparam rows Number of rows in the matrix.
/// @tparam columns Number of columns in the matrix.
///
/// @related mathfu::Matrix
template <class T, int rows, int columns>
inline Matrix<T, rows, columns> operator*(const T& s,
                                          const Matrix<T, columns, rows>& m) {
  return m * s;
}

/// @brief Multiply a Matrix by a Vector.
///
/// @note Template specialized versions are implemented for 2x2, 3x3, and 4x4
/// matrices to increase performance.  The 3x3 float is also specialized
/// to supported padded the 3-dimensional Vector in SIMD build configurations.
///
/// @param m Matrix to multiply.
/// @param v Vector to multiply.
/// @return Vector containing the result.
///
/// @related mathfu::Matrix
template <class T, int rows, int columns>
inline Vector<T, rows> operator*(const Matrix<T, rows, columns>& m,
                                 const Vector<T, columns>& v) {
  const Vector<T, rows> result(0);
  int offset = 0;
  for (int column = 0; column < columns; column++) {
    for (int row = 0; row < rows; row++) {
      result[row] += m[offset + row] * v[column];
    }
    offset += rows;
  }
  return result;
}

/// @cond MATHFU_INTERNAL
template <class T>
inline Vector<T, 2> operator*(const Matrix<T, 2, 2>& m, const Vector<T, 2>& v) {
  return Vector<T, 2>(m[0] * v[0] + m[2] * v[1], m[1] * v[0] + m[3] * v[1]);
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline Vector<T, 3> operator*(const Matrix<T, 3, 3>& m, const Vector<T, 3>& v) {
  return Vector<T, 3>(MATHFU_MATRIX_3X3_DOT(&m[0], v, 0, 3),
                      MATHFU_MATRIX_3X3_DOT(&m[0], v, 1, 3),
                      MATHFU_MATRIX_3X3_DOT(&m[0], v, 2, 3));
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <>
inline Vector<float, 3> operator*(const Matrix<float, 3, 3>& m,
                                  const Vector<float, 3>& v) {
  return Vector<float, 3>(
      MATHFU_MATRIX_3X3_DOT(&m[0], v, 0, MATHFU_VECTOR_STRIDE_FLOATS(v)),
      MATHFU_MATRIX_3X3_DOT(&m[0], v, 1, MATHFU_VECTOR_STRIDE_FLOATS(v)),
      MATHFU_MATRIX_3X3_DOT(&m[0], v, 2, MATHFU_VECTOR_STRIDE_FLOATS(v)));
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline Vector<T, 4> operator*(const Matrix<T, 4, 4>& m, const Vector<T, 4>& v) {
  return Vector<T, 4>(
      MATHFU_MATRIX_4X4_DOT(&m[0], v, 0), MATHFU_MATRIX_4X4_DOT(&m[0], v, 1),
      MATHFU_MATRIX_4X4_DOT(&m[0], v, 2), MATHFU_MATRIX_4X4_DOT(&m[0], v, 3));
}
/// @endcond

/// @brief Multiply a 4x4 Matrix by a 3-dimensional Vector.
///
/// This is provided as a convenience and assumes the vector has a fourth
/// component equal to 1.
///
/// @param m 4x4 Matrix.
/// @param v 3-dimensional Vector.
/// @return 3-dimensional Vector result.
///
/// @related mathfu::Matrix
template <class T>
inline Vector<T, 3> operator*(const Matrix<T, 4, 4>& m, const Vector<T, 3>& v) {
  Vector<T, 4> v4(v[0], v[1], v[2], 1);
  v4 = m * v4;
  return Vector<T, 3>(v4[0] / v4[3], v4[1] / v4[3], v4[2] / v4[3]);
}

/// @cond MATHFU_INTERNAL
/// @brief Multiply a Matrix with another Matrix.
///
/// @note Template specialized versions are implemented for 2x2, 3x3, and 4x4
/// matrices to improve performance. 3x3 float is also specialized because if
/// SIMD is used the vectors of this type of length 4.
///
/// @param m1 Matrix to multiply.
/// @param m2 Matrix to multiply.
/// @param out_m Pointer to a Matrix which receives the result.
///
/// @tparam T Type of each element in the returned Matrix.
/// @tparam size1 Number of rows in the returned Matrix and columns in m1.
/// @tparam size2 Number of columns in the returned Matrix and rows in m2.
/// @tparam size3 Number of columns in m3.
template <class T, int size1, int size2, int size3>
inline void TimesHelper(const Matrix<T, size1, size2>& m1,
                        const Matrix<T, size2, size3>& m2,
                        Matrix<T, size1, size3>* out_m) {
  for (int i = 0; i < size1; i++) {
    for (int j = 0; j < size3; j++) {
      Vector<T, size2> row;
      for (int k = 0; k < size2; k++) {
        row[k] = m1(i, k);
      }
      (*out_m)(i, j) = Vector<T, size2>::DotProduct(m2.GetColumn(j), row);
    }
  }
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline void TimesHelper(const Matrix<T, 2, 2>& m1, const Matrix<T, 2, 2>& m2,
                        Matrix<T, 2, 2>* out_m) {
  Matrix<T, 2, 2>& out = *out_m;
  out[0] = m1[0] * m2[0] + m1[2] * m2[1];
  out[1] = m1[1] * m2[0] + m1[3] * m2[1];
  out[2] = m1[0] * m2[2] + m1[2] * m2[3];
  out[3] = m1[1] * m2[2] + m1[3] * m2[3];
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <typename T>
inline void TimesHelper(const Matrix<T, 3, 3>& m1, const Matrix<T, 3, 3>& m2,
                        Matrix<T, 3, 3>* out_m) {
  Matrix<T, 3, 3>& out = *out_m;
  {
    Vector<T, 3> row(m1[0], m1[3], m1[6]);
    out[0] = Vector<T, 3>::DotProduct(m2.GetColumn(0), row);
    out[3] = Vector<T, 3>::DotProduct(m2.GetColumn(1), row);
    out[6] = Vector<T, 3>::DotProduct(m2.GetColumn(2), row);
  }
  {
    Vector<T, 3> row(m1[1], m1[4], m1[7]);
    out[1] = Vector<T, 3>::DotProduct(m2.GetColumn(0), row);
    out[4] = Vector<T, 3>::DotProduct(m2.GetColumn(1), row);
    out[7] = Vector<T, 3>::DotProduct(m2.GetColumn(2), row);
  }
  {
    Vector<T, 3> row(m1[2], m1[5], m1[8]);
    out[2] = Vector<T, 3>::DotProduct(m2.GetColumn(0), row);
    out[5] = Vector<T, 3>::DotProduct(m2.GetColumn(1), row);
    out[8] = Vector<T, 3>::DotProduct(m2.GetColumn(2), row);
  }
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline void TimesHelper(const Matrix<T, 4, 4>& m1, const Matrix<T, 4, 4>& m2,
                        Matrix<T, 4, 4>* out_m) {
  Matrix<T, 4, 4>& out = *out_m;
  {
    Vector<T, 4> row(m1[0], m1[4], m1[8], m1[12]);
    out[0] = Vector<T, 4>::DotProduct(m2.GetColumn(0), row);
    out[4] = Vector<T, 4>::DotProduct(m2.GetColumn(1), row);
    out[8] = Vector<T, 4>::DotProduct(m2.GetColumn(2), row);
    out[12] = Vector<T, 4>::DotProduct(m2.GetColumn(3), row);
  }
  {
    Vector<T, 4> row(m1[1], m1[5], m1[9], m1[13]);
    out[1] = Vector<T, 4>::DotProduct(m2.GetColumn(0), row);
    out[5] = Vector<T, 4>::DotProduct(m2.GetColumn(1), row);
    out[9] = Vector<T, 4>::DotProduct(m2.GetColumn(2), row);
    out[13] = Vector<T, 4>::DotProduct(m2.GetColumn(3), row);
  }
  {
    Vector<T, 4> row(m1[2], m1[6], m1[10], m1[14]);
    out[2] = Vector<T, 4>::DotProduct(m2.GetColumn(0), row);
    out[6] = Vector<T, 4>::DotProduct(m2.GetColumn(1), row);
    out[10] = Vector<T, 4>::DotProduct(m2.GetColumn(2), row);
    out[14] = Vector<T, 4>::DotProduct(m2.GetColumn(3), row);
  }
  {
    Vector<T, 4> row(m1[3], m1[7], m1[11], m1[15]);
    out[3] = Vector<T, 4>::DotProduct(m2.GetColumn(0), row);
    out[7] = Vector<T, 4>::DotProduct(m2.GetColumn(1), row);
    out[11] = Vector<T, 4>::DotProduct(m2.GetColumn(2), row);
    out[15] = Vector<T, 4>::DotProduct(m2.GetColumn(3), row);
  }
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// @brief Compute the identity matrix.
///
/// @note There are template specializations for 2x2, 3x3, and 4x4 matrices to
/// increase performance.
///
/// @return Identity Matrix.
/// @tparam T Type of each element in the returned Matrix.
/// @tparam rows Number of rows in the returned Matrix.
/// @tparam columns Number of columns in the returned Matrix.
template <class T, int rows, int columns>
inline Matrix<T, rows, columns> IdentityHelper() {
  Matrix<T, rows, columns> return_matrix(0.f);
  int min_d = rows < columns ? rows : columns;
  for (int i = 0; i < min_d; ++i) return_matrix(i, i) = 1;
  return return_matrix;
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline Matrix<T, 2, 2> IdentityHelper() {
  return Matrix<T, 2, 2>(1, 0, 0, 1);
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline Matrix<T, 3, 3> IdentityHelper() {
  return Matrix<T, 3, 3>(1, 0, 0, 0, 1, 0, 0, 0, 1);
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline Matrix<T, 4, 4> IdentityHelper() {
  return Matrix<T, 4, 4>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// @brief Compute the outer product of two vectors.
///
/// @note There are template specialization for 2x2, 3x3, and 4x4 matrices to
/// increase performance.
template <class T, int rows, int columns>
static inline Matrix<T, rows, columns> OuterProductHelper(
    const Vector<T, rows>& v1, const Vector<T, columns>& v2) {
  Matrix<T, rows, columns> result(0);
  int offset = 0;
  for (int column = 0; column < columns; column++) {
    for (int row = 0; row < rows; row++) {
      result[row + offset] = v1[row] * v2[column];
    }
    offset += rows;
  }
  return result;
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
static inline Matrix<T, 2, 2> OuterProductHelper(const Vector<T, 2>& v1,
                                                 const Vector<T, 2>& v2) {
  return Matrix<T, 2, 2>(v1[0] * v2[0], v1[1] * v2[0], v1[0] * v2[1],
                         v1[1] * v2[1]);
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
static inline Matrix<T, 3, 3> OuterProductHelper(const Vector<T, 3>& v1,
                                                 const Vector<T, 3>& v2) {
  return Matrix<T, 3, 3>(v1[0] * v2[0], v1[1] * v2[0], v1[2] * v2[0],
                         v1[0] * v2[1], v1[1] * v2[1], v1[2] * v2[1],
                         v1[0] * v2[2], v1[1] * v2[2], v1[2] * v2[2]);
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
static inline Matrix<T, 4, 4> OuterProductHelper(const Vector<T, 4>& v1,
                                                 const Vector<T, 4>& v2) {
  return Matrix<T, 4, 4>(
      v1[0] * v2[0], v1[1] * v2[0], v1[2] * v2[0], v1[3] * v2[0], v1[0] * v2[1],
      v1[1] * v2[1], v1[2] * v2[1], v1[3] * v2[1], v1[0] * v2[2], v1[1] * v2[2],
      v1[2] * v2[2], v1[3] * v2[2], v1[0] * v2[3], v1[1] * v2[3], v1[2] * v2[3],
      v1[3] * v2[3]);
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// Struct used for template specialization for functions that
/// returns constants.
template <class T>
class Constants {
 public:
  /// Minimum absolute value of the determinant of an invertible matrix.
  static T GetDeterminantThreshold() {
    // No constant defined for the general case.
    assert(false);
    return 0;
  }
};
/// @endcond

/// Functions that return constants for <code>float</code> values.
template <>
class Constants<float> {
 public:
  /// @brief Minimum absolute value of the determinant of an invertible
  /// <code>float</code> Matrix.
  ///
  /// <code>float</code> values have 23 bits of precision which is roughly
  /// 1e7f, given that the final step of matrix inversion is multiplication
  /// with the inverse of the determinant, the minimum value of the
  /// determinant is 1e-7f before the precision too low to accurately
  /// calculate the inverse.
  /// @returns Minimum absolute value of the determinant of an invertible
  /// <code>float</code> Matrix.
  ///
  /// @related mathfu::Matrix::InverseWithDeterminantCheck()
  static float GetDeterminantThreshold() { return 1e-7f; }
};

/// Functions that return constants for <code>double</code> values.
template <>
class Constants<double> {
 public:
  /// @brief Minimum absolute value of the determinant of an invertible
  /// <code>double</code> Matrix.
  ///
  /// <code>double</code> values have 46 bits of precision which is roughly
  /// 1e15, given that the final step of matrix inversion is multiplication
  /// with the inverse of the determinant, the minimum value of the
  /// determinant is 1e-15 before the precision too low to accurately
  /// calculate the inverse.
  /// @returns Minimum absolute value of the determinant of an invertible
  /// <code>double</code> Matrix.
  ///
  /// @related mathfu::Matrix::InverseWithDeterminantCheck()
  static double GetDeterminantThreshold() { return 1e-15; }
};

/// @cond MATHFU_INTERNAL
/// @brief Compute the inverse of a matrix.
///
/// There is template specialization  for 2x2, 3x3, and 4x4 matrices to
/// increase performance. Inverse is not implemented for dense matrices that
/// are not of size 2x2, 3x3, and 4x4.  If check_invertible is true the
/// determine of the matrix is compared with
/// Constants<T>::GetDeterminantThreshold() to roughly determine whether the
/// Matrix is invertible.
template <bool check_invertible, class T, int rows, int columns>
inline bool InverseHelper(const Matrix<T, rows, columns>& m,
                          Matrix<T, rows, columns>* const inverse) {
  assert(false);
  (void)m;
  *inverse = T::Identity();
  return false;
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <bool check_invertible, class T>
inline bool InverseHelper(const Matrix<T, 2, 2>& m,
                          Matrix<T, 2, 2>* const inverse) {
  T determinant = m[0] * m[3] - m[1] * m[2];
  if (check_invertible &&
      fabs(determinant) < Constants<T>::GetDeterminantThreshold()) {
    return false;
  }
  T inverseDeterminant = 1 / determinant;
  (*inverse)[0] = inverseDeterminant * m[3];
  (*inverse)[1] = -inverseDeterminant * m[1];
  (*inverse)[2] = -inverseDeterminant * m[2];
  (*inverse)[3] = inverseDeterminant * m[0];
  return true;
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <bool check_invertible, class T>
inline bool InverseHelper(const Matrix<T, 3, 3>& m,
                          Matrix<T, 3, 3>* const inverse) {
  // Find determinant of matrix.
  T sub11 = m[4] * m[8] - m[5] * m[7], sub12 = -m[1] * m[8] + m[2] * m[7],
    sub13 = m[1] * m[5] - m[2] * m[4];
  T determinant = m[0] * sub11 + m[3] * sub12 + m[6] * sub13;
  if (check_invertible &&
      fabs(determinant) < Constants<T>::GetDeterminantThreshold()) {
    return false;
  }
  // Find determinants of 2x2 submatrices for the elements of the inverse.
  *inverse = Matrix<T, 3, 3>(
      sub11, sub12, sub13, m[6] * m[5] - m[3] * m[8], m[0] * m[8] - m[6] * m[2],
      m[3] * m[2] - m[0] * m[5], m[3] * m[7] - m[6] * m[4],
      m[6] * m[1] - m[0] * m[7], m[0] * m[4] - m[3] * m[1]);
  *(inverse) *= 1 / determinant;
  return true;
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
inline int FindLargestPivotElem(const Matrix<T, 4, 4>& m) {
  Vector<T, 4> fabs_column(fabs(m[0]), fabs(m[1]), fabs(m[2]), fabs(m[3]));
  if (fabs_column[0] > fabs_column[1]) {
    if (fabs_column[0] > fabs_column[2]) {
      if (fabs_column[0] > fabs_column[3]) {
        return 0;
      } else {
        return 3;
      }
    } else if (fabs_column[2] > fabs_column[3]) {
      return 2;
    } else {
      return 3;
    }
  } else if (fabs_column[1] > fabs_column[2]) {
    if (fabs_column[1] > fabs_column[3]) {
      return 1;
    } else {
      return 3;
    }
  } else if (fabs_column[2] > fabs_column[3]) {
    return 2;
  } else {
    return 3;
  }
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <bool check_invertible, class T>
bool InverseHelper(const Matrix<T, 4, 4>& m, Matrix<T, 4, 4>* const inverse) {
  // This will find the pivot element.
  int pivot_elem = FindLargestPivotElem(m);
  // This will perform the pivot and find the row, column, and 3x3 submatrix
  // for this pivot.
  Vector<T, 3> row, column;
  Matrix<T, 3> matrix;
  if (pivot_elem == 0) {
    row = Vector<T, 3>(m[4], m[8], m[12]);
    column = Vector<T, 3>(m[1], m[2], m[3]);
    matrix =
        Matrix<T, 3>(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]);
  } else if (pivot_elem == 1) {
    row = Vector<T, 3>(m[5], m[9], m[13]);
    column = Vector<T, 3>(m[0], m[2], m[3]);
    matrix =
        Matrix<T, 3>(m[4], m[6], m[7], m[8], m[10], m[11], m[12], m[14], m[15]);
  } else if (pivot_elem == 2) {
    row = Vector<T, 3>(m[6], m[10], m[14]);
    column = Vector<T, 3>(m[0], m[1], m[3]);
    matrix =
        Matrix<T, 3>(m[4], m[5], m[7], m[8], m[9], m[11], m[12], m[13], m[15]);
  } else {
    row = Vector<T, 3>(m[7], m[11], m[15]);
    column = Vector<T, 3>(m[0], m[1], m[2]);
    matrix =
        Matrix<T, 3>(m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14]);
  }
  T pivot_value = m[pivot_elem];
  if (check_invertible &&
      fabs(pivot_value) < Constants<T>::GetDeterminantThreshold()) {
    return false;
  }
  // This will compute the inverse using the row, column, and 3x3 submatrix.
  T inv = -1 / pivot_value;
  row *= inv;
  matrix += Matrix<T, 3>::OuterProduct(column, row);
  Matrix<T, 3> mat_inverse;
  if (!InverseHelper<check_invertible>(matrix, &mat_inverse) &&
      check_invertible) {
    return false;
  }
  Vector<T, 3> col_inverse = mat_inverse * (column * inv);
  Vector<T, 3> row_inverse = row * mat_inverse;
  T pivot_inverse = Vector<T, 3>::DotProduct(row, col_inverse) - inv;
  if (pivot_elem == 0) {
    *inverse = Matrix<T, 4, 4>(
        pivot_inverse, col_inverse[0], col_inverse[1], col_inverse[2],
        row_inverse[0], mat_inverse[0], mat_inverse[1], mat_inverse[2],
        row_inverse[1], mat_inverse[3], mat_inverse[4], mat_inverse[5],
        row_inverse[2], mat_inverse[6], mat_inverse[7], mat_inverse[8]);
  } else if (pivot_elem == 1) {
    *inverse = Matrix<T, 4, 4>(
        row_inverse[0], mat_inverse[0], mat_inverse[1], mat_inverse[2],
        pivot_inverse, col_inverse[0], col_inverse[1], col_inverse[2],
        row_inverse[1], mat_inverse[3], mat_inverse[4], mat_inverse[5],
        row_inverse[2], mat_inverse[6], mat_inverse[7], mat_inverse[8]);
  } else if (pivot_elem == 2) {
    *inverse = Matrix<T, 4, 4>(
        row_inverse[0], mat_inverse[0], mat_inverse[1], mat_inverse[2],
        row_inverse[1], mat_inverse[3], mat_inverse[4], mat_inverse[5],
        pivot_inverse, col_inverse[0], col_inverse[1], col_inverse[2],
        row_inverse[2], mat_inverse[6], mat_inverse[7], mat_inverse[8]);
  } else {
    *inverse = Matrix<T, 4, 4>(
        row_inverse[0], mat_inverse[0], mat_inverse[1], mat_inverse[2],
        row_inverse[1], mat_inverse[3], mat_inverse[4], mat_inverse[5],
        row_inverse[2], mat_inverse[6], mat_inverse[7], mat_inverse[8],
        pivot_inverse, col_inverse[0], col_inverse[1], col_inverse[2]);
  }
  return true;
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// Create a 4x4 perpective matrix.
template <class T>
inline Matrix<T, 4, 4> PerspectiveHelper(T fovy, T aspect, T znear, T zfar,
                                         T handedness) {
  const T y = 1 / std::tan(fovy * static_cast<T>(.5));
  const T x = y / aspect;
  const T zdist = (znear - zfar);
  const T zfar_per_zdist = zfar / zdist;
  return Matrix<T, 4, 4>(x, 0, 0, 0, 0, y, 0, 0, 0, 0,
                         zfar_per_zdist * handedness, -1 * handedness, 0, 0,
                         2.0f * znear * zfar_per_zdist, 0);
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// Create a 4x4 orthographic matrix.
template <class T>
static inline Matrix<T, 4, 4> OrthoHelper(T left, T right, T bottom, T top,
                                          T znear, T zfar, T handedness) {
  return Matrix<T, 4, 4>(static_cast<T>(2) / (right - left), 0, 0, 0, 0,
                         static_cast<T>(2) / (top - bottom), 0, 0, 0, 0,
                         -handedness * static_cast<T>(2) / (zfar - znear), 0,
                         -(right + left) / (right - left),
                         -(top + bottom) / (top - bottom),
                         -(zfar + znear) / (zfar - znear), static_cast<T>(1));
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// Calculate the axes required to construct a 3-dimensional camera matrix that
/// looks at "at" from eye position "eye" with the up vector "up".  The axes
/// are returned in a 4 element "axes" array.
template <class T>
static void LookAtHelperCalculateAxes(const Vector<T, 3>& at,
                                      const Vector<T, 3>& eye,
                                      const Vector<T, 3>& up, T handedness,
                                      Vector<T, 3>* const axes) {
  // Notice that y-axis is always the same regardless of handedness.
  axes[2] = (at - eye).Normalized();
  axes[0] = Vector<T, 3>::CrossProduct(up, axes[2]).Normalized();
  axes[1] = Vector<T, 3>::CrossProduct(axes[2], axes[0]);
  axes[3] = Vector<T, 3>(handedness * Vector<T, 3>::DotProduct(axes[0], eye),
                         -Vector<T, 3>::DotProduct(axes[1], eye),
                         handedness * Vector<T, 3>::DotProduct(axes[2], eye));

  // Default calculation is left-handed (i.e. handedness=-1).
  // Negate x and z axes for right-handed (i.e. handedness=+1) case.
  const T neg = -handedness;
  axes[0] *= neg;
  axes[2] *= neg;
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// Create a 3-dimensional camera matrix.
template <class T>
static inline Matrix<T, 4, 4> LookAtHelper(const Vector<T, 3>& at,
                                           const Vector<T, 3>& eye,
                                           const Vector<T, 3>& up,
                                           T handedness) {
  Vector<T, 3> axes[4];
  LookAtHelperCalculateAxes(at, eye, up, handedness, axes);
  const Vector<T, 4> column0(axes[0][0], axes[1][0], axes[2][0], 0);
  const Vector<T, 4> column1(axes[0][1], axes[1][1], axes[2][1], 0);
  const Vector<T, 4> column2(axes[0][2], axes[1][2], axes[2][2], 0);
  const Vector<T, 4> column3(axes[3], 1);
  return Matrix<T, 4, 4>(column0, column1, column2, column3);
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// Get the 3D position in object space from a window coordinate.
template <class T>
static inline bool UnProjectHelper(const Vector<T, 3>& window_coord,
                                   const Matrix<T, 4, 4>& model_view,
                                   const Matrix<T, 4, 4>& projection,
                                   const float window_width,
                                   const float window_height,
                                   Vector<T, 3>& result) {
  if (window_coord.z < static_cast<T>(0) ||
      window_coord.z > static_cast<T>(1)) {
    // window_coord.z should be with in [0, 1]
    // 0: near plane
    // 1: far plane
    return false;
  }
  Matrix<T, 4, 4> matrix = (projection * model_view).Inverse();
  Vector<T, 4> standardized = Vector<T, 4>(
      static_cast<T>(2) * (window_coord.x - window_width) / window_width +
          static_cast<T>(1),
      static_cast<T>(2) * (window_coord.y - window_height) / window_height +
          static_cast<T>(1),
      static_cast<T>(2) * window_coord.z - static_cast<T>(1),
      static_cast<T>(1));

  Vector<T, 4> multiply = matrix * standardized;
  if (multiply.w == static_cast<T>(0)) {
    return false;
  }
  result = multiply.xyz() / multiply.w;
  return true;
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <typename T, int rows, int columns, typename CompatibleT>
static inline Matrix<T, rows, columns> FromTypeHelper(const CompatibleT& compatible) {
// C++11 is required for constructed unions.
#if __cplusplus >= 201103L
  // Use a union instead of reinterpret_cast to avoid aliasing bugs.
  union ConversionUnion {
    ConversionUnion() {}  // C++11.
    CompatibleT compatible;
    VectorPacked<T, rows> packed[columns];
  } u;
  static_assert(sizeof(u.compatible) == sizeof(u.packed), "Conversion size mismatch.");

  // The read of `compatible` and write to `u.compatible` gets optimized away,
  // and this becomes essentially a safe reinterpret_cast.
  u.compatible = compatible;

  // Call the packed vector constructor with the `compatible` data.
  return Matrix<T, rows, columns>(u.packed);
#else
  // Use the less-desirable memcpy technique if C++11 is not available.
  // Most compilers understand memcpy deep enough to avoid replace the function
  // call with a series of load/stores, which should then get optimized away,
  // however in the worst case the optimize away may not happen.
  // Note: Memcpy avoids aliasing bugs because it operates via unsigned char*,
  // which is allowed to alias any type.
  // See:
  // http://stackoverflow.com/questions/15745030/type-punning-with-void-without-breaking-the-strict-aliasing-rule-in-c99
  Matrix<T, rows, columns> m;
  assert(sizeof(m) == sizeof(compatible));
  memcpy(&m, &compatible, sizeof(m));
  return m;
#endif  // __cplusplus >= 201103L
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <typename T, int rows, int columns, typename CompatibleT>
static inline CompatibleT ToTypeHelper(const Matrix<T, rows, columns>& m) {
// See FromTypeHelper() for comments.
#if __cplusplus >= 201103L
  union ConversionUnion {
    ConversionUnion() {}
    CompatibleT compatible;
    VectorPacked<T, rows> packed[columns];
  } u;
  static_assert(sizeof(u.compatible) == sizeof(u.packed), "Conversion size mismatch.");
  m.Pack(u.packed);
  return u.compatible;
#else
  CompatibleT compatible;
  assert(sizeof(m) == sizeof(compatible));
  memcpy(&compatible, &m, sizeof(compatible));
  return compatible;
#endif  // __cplusplus >= 201103L
}
/// @endcond

/// @typedef AffineTransform
///
/// @brief A typedef representing a 4x3 float affine transformation.
/// Since the last row ('w' row) of an affine transformation is fixed,
/// this data type only includes the variable information for the transform.
typedef Matrix<float, 4, 3> AffineTransform;
/// @}

}  // namespace mathfu

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Include the specializations to avoid template errors.
// See includes at bottom of vector.h for further explanation.
#include "mathfu/matrix_4x4.h"

#endif  // MATHFU_MATRIX_H_
