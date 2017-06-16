# Copyright 2014 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
cmake_minimum_required(VERSION 2.8.12)

set(PROJECT_NAME MathFu)
project(MathFu)

# Call fplutil to get locations of dependencies and set common build settings.
include("cmake/find_fplutil.cmake")
include("${fplutil_dir}/buildutil/cmake_common.txt")
set_common_build_variables()

# Options that control the build configuration.
# To configure MathFu flags per build target, see the
# mathfu_configure_flags() function.
option(mathfu_enable_simd "Use SIMD implementations when available." ON)
option(mathfu_build_benchmarks "Build MathFu benchmarks." ON)
option(mathfu_build_tests "Build MathFu unit tests." ON)

# Save the mathfu directory, store this in the cache so that it's globally
# accessible from mathfu_configure_flags().
set(mathfu_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "mathfu directory")

# Set build options for ${target} that are required to build with MathFu.
# This takes the optional arguments "enable_simd force_padding".
#
# If enable_simd is specified it configures whether the project should be
# built with SIMD optimized functions.  If force_padding is specified and
# SIMD is enabled, some data structures are padded to work more efficiently
# with SIMD instructions.
#
# If enable_simd is not specified, the mathfu_enable_simd option is used.
# If force_padding isn't specified padding is enabled based upon the
# best general configuration for the target architecture.
function(mathfu_configure_flags target)
  if(fpl_ios)
    set(enable_simd NO)
  else()
    set(enable_simd ${mathfu_enable_simd})
  endif()

  # Add required includes to the target.
  target_include_directories(${target}
    PRIVATE ${mathfu_dir}/include ${dependencies_vectorial_dir}/include)

  # Parse optional arguments.
  set(additional_args ${ARGN})
  list(LENGTH additional_args num_additional_args)
  if(${num_additional_args} GREATER 0)
    list(GET additional_args 0 enable_simd)
  endif()
  if(${num_additional_args} GREATER 1)
    list(GET additional_args 1 force_padding)
  endif()

  # If the SIMD build option is enabled.
  if(enable_simd)
    # Enable SSE4.1 when building with GCC / Clang.
    # NOTE: It's also possible to build the library using using SSE2 with GCC /
    # Clang, change -msse4.1 to -msse2.
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_COMPILER_IS_CLANGXX)
      target_compile_options(${target} PRIVATE -msse4.1)
    endif()
    # Enable SSE2 by default when building with MSVC for 32-bit targets.
    # Note that SSE2 is enabled by default for 64-bit targets, and the
    # compile option will generate an "unknown option" warning.
    if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4)
      target_compile_options(${target} PRIVATE /arch:SSE2)
    endif()
    # Conditionally enable padding.
    if(DEFINED force_padding)
      if(force_padding)
        target_compile_definitions(${target} PRIVATE
          -DMATHFU_COMPILE_FORCE_PADDING=1)
      else()
        target_compile_definitions(${target} PRIVATE
          -DMATHFU_COMPILE_FORCE_PADDING=0)
      endif()
    endif()
  else()
    target_compile_definitions(${target} PRIVATE
      -DMATHFU_COMPILE_WITHOUT_SIMD_SUPPORT)
  endif()
endfunction()

# Modify CMAKE_C_FLAGS and CMAKE_CXX_FLAGS to enable a maximum reasonable
# warning level.
function(mathfu_enable_warnings target)
  get_target_property(target_compile_flags ${target} COMPILE_FLAGS)
  if(MSVC)
    # C4127: conditional expression is constant
    # C4577: 'noexcept' used with no exception handling mode specified.
    target_compile_options(${target} PRIVATE /W4 /WX /wd4127 /wd4577)
  elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
      CMAKE_COMPILER_IS_CLANGXX)
    # Set the maximum warning level for gcc.
    target_compile_options(${target} PRIVATE -Wall -Wextra -Werror
      -Wno-long-long -Wno-variadic-macros)
  endif()
endfunction()

# Macro defined here so that it can be used by all projects included
macro(mathfu_set_ios_attributes project)
  if(fpl_ios)
    set_target_properties(${project} PROPERTIES
      XCODE_ATTRIBUTE_SDKROOT "iphoneos")
    set_target_properties(${project} PROPERTIES
      XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD)")
    set_target_properties(${project} PROPERTIES
      XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
    set_target_properties(${project} PROPERTIES
      XCODE_ATTRIBUTE_VALID_ARCHS "$(ARCHS_STANDARD)")
    set_target_properties(${project} PROPERTIES
      XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "8.0")
  endif()
endmacro(mathfu_set_ios_attributes)

file(GLOB_RECURSE MATHFU_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/mathfu *.h)

if(mathfu_build_benchmarks)
  add_subdirectory(benchmarks)
endif()
if(mathfu_build_tests)
  add_subdirectory(unit_tests)
endif()
