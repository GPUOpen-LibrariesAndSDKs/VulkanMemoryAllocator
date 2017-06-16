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
#ifndef MATHFU_RECT_H_
#define MATHFU_RECT_H_

#include "mathfu/vector.h"

namespace mathfu {

/// @addtogroup mathfu_rect
/// @{
/// @class Rect "mathfu/rect.h"
/// @brief Rect of type T containing position (pos) and width.
///
/// Rect contains two 2D <b>Vector</b>s of type <b>T</b> representing position
/// (pos) and size.
///
/// @tparam T type of Rect elements.
template <class T>
struct Rect {
  Vector<T, 2> pos;
  Vector<T, 2> size;

  /// @brief Create a rect from a vector4 of the same type.
  ///
  /// @param v Vector that the data will be copied from.
  explicit Rect(const Vector<T, 4>& v)
      : pos(v.x, v.y), size(v.z, v.w) {}

  /// @brief Create a rect from x, y, width and height values.
  ///
  /// @param x the given x value.
  /// @param y the given y value.
  /// @param width the given width value.
  /// @param height the given height value.
  inline Rect(T x = static_cast<T>(0), T y = static_cast<T>(0),
              T width = static_cast<T>(0), T height = static_cast<T>(0))
      : pos(x, y), size(width, height) {}

  /// @brief Create a rect from two vector2 representing position and size.
  ///
  /// @param pos Vector representing the position vector (x and y values).
  /// @param size Vector represening the size vector (width and height values).
  inline Rect(const Vector<T, 2>& pos, const Vector<T, 2>& size)
      : pos(pos), size(size) {}
};
/// @}

/// @brief Check if two rects are identical.
///
/// @param r1 Rect to be tested.
/// @param r2 Other rect to be tested.
template <class T>
bool operator==(const Rect<T>& r1, const Rect<T>& r2) {
  return (r1.pos == r2.pos && r1.size == r2.size);
}

/// @brief Check if two rects are <b>not</b> identical.
///
/// @param r1 Rect to be tested.
/// @param r2 Other rect to be tested.
template <class T>
bool operator!=(const Rect<T>& r1, const Rect<T>& r2) {
  return !(r1 == r2);
}

}  // namespace mathfu

#endif  // MATHFU_RECT_H_
