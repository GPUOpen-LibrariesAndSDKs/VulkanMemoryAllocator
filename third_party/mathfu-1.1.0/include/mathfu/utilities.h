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
#ifndef MATHFU_UTILITIES_H_
#define MATHFU_UTILITIES_H_

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <memory>

/// @file mathfu/utilities.h Utilities
/// @brief Utility macros and functions.

/// @addtogroup mathfu_build_config
///
/// By default MathFu will attempt to build with SIMD optimizations enabled
/// based upon the target architecture and compiler options.  However, it's
/// possible to change the default build configuration using the following
/// macros:
///
/// @li @ref MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT
/// @li @ref MATHFU_COMPILE_FORCE_PADDING
///
/// <table>
/// <tr>
///   <th>MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT</th>
///   <th>MATHFU_COMPILE_FORCE_PADDING</th>
///   <th>Configuration</th>
/// </tr>
/// <tr>
///   <td><em>undefined</em></td>
///   <td><em>undefined</em> or 1</td>
///   <td>Default build configuration, SIMD optimization is enabled based upon
///       the target architecture, compiler options and MathFu library
///       support.</td>
/// </tr>
/// <tr>
///   <td><em>undefined</em></td>
///   <td>0</td>
///   <td>If SIMD is supported, padding of data structures is disabled.  See
///       @ref MATHFU_COMPILE_FORCE_PADDING for more information.</td>
/// </tr>
/// <tr>
///   <td><em>defined</em></td>
///   <td><em>undefined/0/1</em></td>
///   <td>Builds MathFu with explicit SIMD optimization disabled.  The compiler
///       could still potentially optimize some code paths with SIMD
///       instructions based upon the compiler options.</td>
/// </tr>
/// </table>

#ifdef DOXYGEN
/// @addtogroup mathfu_build_config
/// @{
/// @def MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT
/// @brief Disable SIMD build configuration.
///
/// When defined, this macro <b>disables</b> the default behavior of trying to
/// build the library with SIMD enabled  based upon the target architecture
/// and compiler options.
///
/// To use this build option, this macro <b>must</b> be defined in all modules
/// of the project.
#define MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT
/// @}
#endif  // DOXYGEN
#if !defined(MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT)
#if defined(__SSE__)
#define MATHFU_COMPILE_WITH_SIMD
#elif defined(__ARM_NEON__)
#define MATHFU_COMPILE_WITH_SIMD
#elif defined(_M_IX86_FP)  // MSVC
#if _M_IX86_FP >= 1        // SSE enabled
#define MATHFU_COMPILE_WITH_SIMD
#endif  // _M_IX86_FP >= 1
#endif
#endif  // !defined(MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT)

#ifdef DOXYGEN
/// @addtogroup mathfu_build_config
/// @{
/// @def MATHFU_COMPILE_FORCE_PADDING
/// @brief Enable / disable padding of data structures.
///
/// By default, when @ref MATHFU_COMPILE_FORCE_PADDING is <b>not</b> defined,
/// data structures are padded when SIMD is enabled
/// (i.e when @ref MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT is also not defined).
///
/// If @ref MATHFU_COMPILE_FORCE_PADDING is defined as <b>1</b>, all data
/// structures are padded to a power of 2 size which enables more efficient
/// SIMD operations.  This  is the default build configuration when SIMD is
/// enabled.
///
/// If @ref MATHFU_COMPILE_FORCE_PADDING is defined as <b>0</b>, all data
/// structures are packed by the compiler (with no padding) even when the SIMD
/// build configuration is enabled.  This build option can be useful in the
/// rare occasion an application is CPU memory bandwidth constrained, at the
/// expense of additional instructions to copy to / from SIMD registers.
///
/// To use this build option, this macro <b>must</b> be defined in all modules
/// of the project.
///
/// @see MATHFU_COMPILE_WITHOUT_SIMD_SUPPORT
#define MATHFU_COMPILE_FORCE_PADDING
/// @}
#endif  // DOXYGEN

#ifdef MATHFU_COMPILE_WITH_SIMD
/// @cond MATHFU_INTERNAL
/// @addtogroup mathfu_build_config
/// @{
/// @def MATHFU_COMPILE_WITH_PADDING
/// @brief Enable padding of data structures to be efficient with SIMD.
///
/// When defined, this option enables padding of some data structures (e.g
/// @ref vec3) to be more efficient with SIMD operations.  This option is
/// only applicable when @ref MATHFU_COMPILE_WITHOUT_SIMD is not defined and
/// the target architecture and compiler support SIMD.
///
/// To use this build option, this macro <b>must</b> be defined in all modules
/// of the project.
/// @see MATHFU_COMPILE_FORCE_PADDING
#define MATHFU_COMPILE_WITH_PADDING
/// @}
#if defined(MATHFU_COMPILE_FORCE_PADDING)
#if MATHFU_COMPILE_FORCE_PADDING == 1
#if !defined(MATHFU_COMPILE_WITH_PADDING)
#define MATHFU_COMPILE_WITH_PADDING
#endif  // !defined(MATHFU_COMPILE_WITH_PADDING)
#else
#if defined(MATHFU_COMPILE_WITH_PADDING)
#undef MATHFU_COMPILE_WITH_PADDING
#endif  // MATHFU_COMPILE_WITH_PADDING
#endif  // MATHFU_COMPILE_FORCE_PADDING == 1
#endif  // MATHFU_COMPILE_FORCE_PADDING
/// @endcond
#endif  // MATHFU_COMPILE_WITH_SIMD

/// @addtogroup mathfu_version
/// @{

/// @def MATHFU_VERSION_MAJOR
/// @brief Major version number of the library.
/// @see kMathFuVersionString
#define MATHFU_VERSION_MAJOR 1
/// @def MATHFU_VERSION_MINOR
/// @brief Minor version number of the library.
/// @see kMathFuVersionString
#define MATHFU_VERSION_MINOR 1
/// @def MATHFU_VERSION_REVISION
/// @brief Revision number of the library.
/// @see kMathFuVersionString
#define MATHFU_VERSION_REVISION 0

/// @}

/// @cond MATHFU_INTERNAL
#define MATHFU_STRING_EXPAND(X) #X
#define MATHFU_STRING(X) MATHFU_STRING_EXPAND(X)
/// @endcond

/// @cond MATHFU_INTERNAL
// Generate string which contains build options for the library.
#if defined(MATHFU_COMPILE_WITH_SIMD)
#define MATHFU_BUILD_OPTIONS_SIMD "[simd]"
#else
#define MATHFU_BUILD_OPTIONS_SIMD "[no simd]"
#endif  // defined(MATHFU_COMPILE_WITH_SIMD)
#if defined(MATHFU_COMPILE_WITH_PADDING)
#define MATHFU_BUILD_OPTIONS_PADDING "[padding]"
#else
#define MATHFU_BUILD_OPTIONS_PADDING "[no padding]"
#endif  // defined(MATHFU_COMPILE_WITH_PADDING)
/// @endcond

/// @addtogroup mathfu_version
/// @{
/// @def MATHFU_BUILD_OPTIONS_STRING
/// @brief String that describes the library's build configuration.
#define MATHFU_BUILD_OPTIONS_STRING \
  (MATHFU_BUILD_OPTIONS_SIMD " " MATHFU_BUILD_OPTIONS_PADDING)
/// @}

// Weak linkage is culled by VS & doesn't work on cygwin.
#if !defined(_WIN32) && !defined(__CYGWIN__)

extern volatile __attribute__((weak)) const char *kMathFuVersionString;
/// @addtogroup mathfu_version
/// @{

/// @var kMathFuVersionString
/// @brief String which identifies the current version of MathFu.
///
/// @ref kMathFuVersionString is used by Google developers to identify which
/// applications uploaded to Google Play are using this library.  This allows
/// the development team at Google to determine the popularity of the library.
/// How it works: Applications that are uploaded to the Google Play Store are
/// scanned for this version string.  We track which applications are using it
/// to measure popularity.  You are free to remove it (of course) but we would
/// appreciate if you left it in.
///
/// @see MATHFU_VERSION_MAJOR
/// @see MATHFU_VERSION_MINOR
/// @see MATHFU_VERSION_REVISION
volatile __attribute__((weak)) const char *kMathFuVersionString =
    "MathFu " MATHFU_STRING(MATHFU_VERSION_MAJOR) "." MATHFU_STRING(
        MATHFU_VERSION_MINOR) "." MATHFU_STRING(MATHFU_VERSION_REVISION);
/// @}

#endif  // !defined(_WIN32) && !defined(__CYGWIN__)

/// @cond MATHFU_INTERNAL
template <bool>
struct static_assert_util;
template <>
struct static_assert_util<true> {};
/// @endcond

/// @addtogroup mathfu_utilities
/// @{
/// @def MATHFU_STATIC_ASSERT
/// @brief Compile time assert for pre-C++11 compilers.
///
/// For example:
/// <blockquote><code>
/// MATHFU_STATIC_ASSERT(0 == 1);
/// </code></blockquote> will result in a compile error.
#define MATHFU_STATIC_ASSERT(x) static_assert_util<(x)>()
/// @}

/// @cond MATHFU_INTERNAL
/// Unroll an loop up to 4 iterations, where iterator is the identifier
/// used in each operation (e.g "i"), number_of_iterations is a constant which
/// specifies the number of times to perform the operation and "operation" is
/// the statement to execute for each iteration of the loop (e.g data[i] = v).
#define MATHFU_UNROLLED_LOOP(iterator, number_of_iterations, operation) \
  {                                                                     \
    const int iterator = 0;                                             \
    { operation; }                                                      \
    if ((number_of_iterations) > 1) {                                   \
      const int iterator = 1;                                           \
      { operation; }                                                    \
      if ((number_of_iterations) > 2) {                                 \
        const int iterator = 2;                                         \
        { operation; }                                                  \
        if ((number_of_iterations) > 3) {                               \
          const int iterator = 3;                                       \
          { operation; }                                                \
          if ((number_of_iterations) > 4) {                             \
            for (int iterator = 4; iterator < (number_of_iterations);   \
                 ++iterator) {                                          \
              operation;                                                \
            }                                                           \
          }                                                             \
        }                                                               \
      }                                                                 \
    }                                                                   \
  }
/// @endcond

namespace mathfu {

/// @addtogroup mathfu_utilities
/// @{

/// @brief Clamp x within [lower, upper].
/// @anchor mathfu_Clamp
///
/// @note Results are undefined if lower > upper.
///
/// @param x Value to clamp.
/// @param lower Lower value of the range.
/// @param upper Upper value of the range.
/// @returns Clamped value.
template <class T>
T Clamp(const T &x, const T &lower, const T &upper) {
  return std::max<T>(lower, std::min<T>(x, upper));
}

/// @brief Linearly interpolate between range_start and range_end, based on
/// percent.
/// @anchor mathfu_Lerp
///
/// @param range_start Start of the range.
/// @param range_end End of the range.
/// @param percent Value between 0.0 and 1.0 used to interpolate between
/// range_start and range_end.  Where a value of 0.0 results in a return
/// value of range_start and 1.0 results in a return value of range_end.
/// @return Value between range_start and range_end.
///
/// @tparam T Type of the range to interpolate over.
/// @tparam T2 Type of the value used to perform interpolation
///         (e.g float or double).
template <class T, class T2>
T Lerp(const T &range_start, const T &range_end, const T2 &percent) {
  const T2 one_minus_percent = static_cast<T2>(1.0) - percent;
  return range_start * one_minus_percent + range_end * percent;
}

/// @brief Linearly interpolate between range_start and range_end, based on
/// percent.
/// @anchor mathfu_Lerp2
///
/// @param range_start Start of the range.
/// @param range_end End of the range.
/// @param percent Value between 0.0 and 1.0 used to interpolate between
/// range_start and range_end.  Where a value of 0.0 results in a return
/// value of range_start and 1.0 results in a return value of range_end.
/// @return Value between range_start and range_end.
///
/// @tparam T Type of the range to interpolate over.
template <class T>
T Lerp(const T &range_start, const T &range_end, const T &percent) {
  return Lerp<T, T>(range_start, range_end, percent);
}

/// @brief Check if val is within [range_start..range_end).
/// @anchor mathfu_InRange
///
/// @param val Value to be tested.
/// @param range_start Starting point of the range (inclusive).
/// @param range_end Ending point of the range (non-inclusive).
/// @return Bool indicating success.
///
/// @tparam T Type of values to test.
template <class T>
bool InRange(T val, T range_start, T range_end) {
  return val >= range_start && val < range_end;
}

/// @brief  Generate a random value of type T.
/// @anchor mathfu_Random
///
/// This method generates a random value of type T, greater than or equal to
/// 0.0 and less than 1.0.
///
/// This function uses the standard C library function rand() from math.h to
/// generate the random number.
///
/// @returns Random number greater than or equal to 0.0 and less than 1.0.
///
/// @see RandomRange()
/// @see RandomInRange()
template <class T>
inline T Random() {
  return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
}

/// @cond MATHFU_INTERNAL
template <>
inline float Random() {
  return static_cast<float>(rand() >> 8) /
         (static_cast<float>((RAND_MAX >> 8) + 1));
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <>
inline double Random() {
  return static_cast<double>(rand()) / (static_cast<double>(RAND_MAX + 1LL));
}
/// @endcond

/// @brief Generate a random value of type T in the range -range...+range
/// @anchor mathfu_RandomRange
///
/// This function uses the standard C library function rand() from math.h to
/// generate the random number.
///
/// @param range Range of the random value to generate.
/// @return Random value in the range -range to +range
///
/// @see Random()
template <class T>
inline T RandomRange(T range) {
  return (Random<T>() * range * 2) - range;
}

/// @brief Generate a random number between [range_start, range_end]
/// @anchor mathfu_RandomInRange
///
/// This function uses the standard C library function rand() from math.h to
/// generate the random number.
///
/// @param range_start Minimum value.
/// @param range_end Maximum value.
/// @return Random value in the range [range_start, range_end].
///
/// @see Lerp()
/// @see Random()
template <class T>
inline T RandomInRange(T range_start, T range_end) {
  return Lerp(range_start, range_end, Random<T>());
}

/// @cond MATHFU_INTERNAL
template <>
inline int RandomInRange<int>(int range_start, int range_end) {
  return static_cast<int>(RandomInRange<float>(static_cast<float>(range_start),
                                               static_cast<float>(range_end)));
}
/// @endcond

/// @brief Round a value up to the nearest power of 2.
///
/// @param x Value to round up.
/// @returns Value rounded up to the nearest power of 2.
template <class T>
T RoundUpToPowerOf2(T x) {
  return static_cast<T>(
      pow(static_cast<T>(2), ceil(log(x) / log(static_cast<T>(2)))));
}

/// @brief Specialized version of RoundUpToPowerOf2 for int32_t.
template <>
inline int32_t RoundUpToPowerOf2<>(int32_t x) {
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
}

/// @brief Round a value up to the type's size boundary.
///
/// @param v Value to round up.
/// @returns Value rounded up to the type's size boundary.
template <typename T>
uint32_t RoundUpToTypeBoundary(uint32_t v) {
  return (v + sizeof(T) - 1) & ~(sizeof(T) - 1);
}

/// @}

/// @addtogroup mathfu_allocator
///
/// If you use MathFu with SIMD (SSE in particular), you need to have all
/// your allocations be 16-byte aligned (which isn't the case with the default
/// allocators on most platforms except OS X).
///
/// You can either use simd_allocator, which solves the problem for
/// any STL containers, but not for manual dynamic allocations or the
/// new/delete override MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE will
/// solve it for all allocations, at the cost of MATHFU_ALIGNMENT bytes per
/// allocation.

/// @addtogroup mathfu_allocator
/// @{

/// @def MATHFU_ALIGNMENT
/// @brief Alignment (in bytes) of memory allocated by AllocateAligned.
///
/// @see mathfu::AllocateAligned()
/// @see mathfu::simd_allocator
#define MATHFU_ALIGNMENT 16

/// @brief Allocate an aligned block of memory.
/// @anchor mathfu_AllocateAligned
///
/// This function allocates a block of memory aligned to MATHFU_ALIGNMENT
/// bytes.
///
/// @param n Size of memory to allocate.
/// @return Pointer to aligned block of allocated memory or NULL if
/// allocation failed.
inline void *AllocateAligned(size_t n) {
#if defined(_MSC_VER) && _MSC_VER >= 1900  // MSVC 2015
  return _aligned_malloc(n, MATHFU_ALIGNMENT);
#else
  // We need to allocate extra bytes to guarantee alignment,
  // and to store the pointer to the original buffer.
  uint8_t *buf = reinterpret_cast<uint8_t *>(malloc(n + MATHFU_ALIGNMENT));
  if (!buf) return NULL;
  // Align to next higher multiple of MATHFU_ALIGNMENT.
  uint8_t *aligned_buf = reinterpret_cast<uint8_t *>(
      (reinterpret_cast<size_t>(buf) + MATHFU_ALIGNMENT) &
      ~(MATHFU_ALIGNMENT - 1));
  // Write out original buffer pointer before aligned buffer.
  // The assert will fail if the allocator granularity is less than the pointer
  // size, or if MATHFU_ALIGNMENT doesn't fit two pointers.
  assert(static_cast<size_t>(aligned_buf - buf) > sizeof(void *));
  *(reinterpret_cast<uint8_t **>(aligned_buf) - 1) = buf;
  return aligned_buf;
#endif  // defined(_MSC_VER) && _MSC_VER >= 1900 // MSVC 2015
}

/// @brief Deallocate a block of memory allocated with AllocateAligned().
/// @anchor mathfu_FreeAligned
///
/// @param p Pointer to memory to deallocate.
inline void FreeAligned(void *p) {
#if defined(_MSC_VER) && _MSC_VER >= 1900  // MSVC 2015
  _aligned_free(p);
#else
  if (p == NULL) return;
  free(*(reinterpret_cast<uint8_t **>(p) - 1));
#endif  // defined(_MSC_VER) && _MSC_VER >= 1900 // MSVC 2015
}

/// @brief SIMD-safe memory allocator, for use with STL types like std::vector.
///
/// For example:
/// <blockquote><code><pre>
/// std::vector<vec4, mathfu::simd_allocator<vec4>> myvector;
/// </pre></code></blockquote>
///
/// @see MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE
/// @tparam T type allocated by this object.
template <typename T>
class simd_allocator : public std::allocator<T> {
 public:
  /// Size type.
  typedef size_t size_type;
  /// Pointer of type T.
  typedef T *pointer;
  /// Const pointer of type T.
  typedef const T *const_pointer;

  /// Constructs a simd_allocator.
  simd_allocator() throw() : std::allocator<T>() {}
  /// @brief Constructs and copies a simd_allocator.
  ///
  /// @param a Allocator to copy.
  simd_allocator(const simd_allocator &a) throw() : std::allocator<T>(a) {}
  /// @brief Constructs and copies a simd_allocator.
  ///
  /// @param a Allocator to copy.
  /// @tparam U type of the object allocated by the allocator to copy.
  template <class U>
  simd_allocator(const simd_allocator<U> &a) throw() : std::allocator<T>(a) {}
  /// @brief Destructs a simd_allocator.
  ~simd_allocator() throw() {}

  /// @brief Obtains an allocator of a different type.
  ///
  /// @tparam  _Tp1 type of the new allocator.
  template <typename _Tp1>
  struct rebind {
    /// @brief Allocator of type _Tp1.
    typedef simd_allocator<_Tp1> other;
  };

  /// @brief Allocate memory for object T.
  ///
  /// @param n Number of types to allocate.
  /// @return Pointer to the newly allocated memory.
  pointer allocate(size_type n) {
    return reinterpret_cast<pointer>(AllocateAligned(n * sizeof(T)));
  }

  /// Deallocate memory referenced by pointer p.
  ///
  /// @param p Pointer to memory to deallocate.
  void deallocate(pointer p, size_type) { FreeAligned(p); }
};

#if defined(_MSC_VER)
#if _MSC_VER <= 1800  // MSVC 2013
#if !defined(noexcept)
#define noexcept
#endif  // !defined(noexcept)
#endif  // _MSC_VER <= 1800
#endif  //  defined(_MSC_VER)

/// @def MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE
/// @brief Macro which overrides the default new and delete allocators.
///
/// To globally override new and delete, simply add the line:
/// <blockquote><code><pre>
/// MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE
/// </pre></code></blockquote>
/// to the end of your main .cpp file.
#define MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE                           \
  void *operator new(std::size_t n) { return mathfu::AllocateAligned(n); }   \
  void *operator new[](std::size_t n) { return mathfu::AllocateAligned(n); } \
  void operator delete(void *p) noexcept { mathfu::FreeAligned(p); }         \
  void operator delete[](void *p) noexcept { mathfu::FreeAligned(p); }

/// @def MATHFU_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE
/// @brief Macro which defines the new and delete for MathFu classes.
#define MATHFU_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE                       \
  static void *operator new(std::size_t n) {                            \
    return mathfu::AllocateAligned(n);                                  \
  }                                                                     \
  static void *operator new[](std::size_t n) {                          \
    return mathfu::AllocateAligned(n);                                  \
  }                                                                     \
  static void *operator new(std::size_t /*n*/, void *p) { return p; }   \
  static void *operator new[](std::size_t /*n*/, void *p) { return p; } \
  static void operator delete(void *p) { mathfu::FreeAligned(p); }      \
  static void operator delete[](void *p) { mathfu::FreeAligned(p); }    \
  static void operator delete(void * /*p*/, void * /*place*/) {}        \
  static void operator delete[](void * /*p*/, void * /*place*/) {}

/// @}

}  // namespace mathfu

#endif  // MATHFU_UTILITIES_H_
