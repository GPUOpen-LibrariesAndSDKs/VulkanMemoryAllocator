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
cmake_minimum_required(VERSION 2.8)

# Generate a rule to build a performance test executable
# ${benchmark_executable_name} from ${benchmark_source}.  For details of
# additional arguments, see mathfu_configure_flags().
function(benchmark_executable benchmark_executable_name benchmark_source)
  add_executable(${benchmark_executable_name} ${benchmark_source})
  mathfu_configure_flags(${benchmark_executable_name} ${ARGN})
  mathfu_enable_warnings(${benchmark_executable_name})
  target_include_directories(${benchmark_executable_name}
    PRIVATE ${CMAKE_CURRENT_LIST_DIR})
  if(UNIX AND NOT APPLE)
    target_link_libraries(${benchmark_executable_name} rt)
  endif(UNIX AND NOT APPLE)
endfunction()

# Generates a rule to build performance test executables.  This only builds
# ${benchmark_name}_benchmarks if SIMD is disabled (see ${mathfu_enable_simd})
# or ${benchmark_name}_benchmarks and ${benchmark_name}_no_simd_benchmarks if
# SIMD is enabled where the no_simd_benchmarks binary has SIMD disabled.
function(benchmark_executables benchmark_name benchmark_source)
  # Default build options.
  benchmark_executable(${benchmark_name}_benchmarks ${benchmark_source})
  if(mathfu_enable_simd)
    # NOTE: A build configuration below will deliberately duplicate the
    # default build configuration, since these configs could result in
    # different compile time preprocessor code paths.
    # SIMD enabled, padding enabled.
    benchmark_executable(${benchmark_name}_simd_padding_benchmarks
      ${benchmark_source} TRUE TRUE)
    # SIMD enabled, padding disabled.
    benchmark_executable(${benchmark_name}_simd_no_padding_benchmarks
      ${benchmark_source} TRUE FALSE)
    # SIMD disabled, padding disabled.
    benchmark_executable(${benchmark_name}_no_simd_benchmarks
      ${benchmark_source} FALSE FALSE)
  endif()
endfunction()

benchmark_executables(matrix matrix_benchmark/main.cpp)
benchmark_executables(vector vector_benchmark/main.cpp)
