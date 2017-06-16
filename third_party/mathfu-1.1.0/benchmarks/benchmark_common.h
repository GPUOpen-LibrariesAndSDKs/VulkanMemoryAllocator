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
#ifndef MATHFU_BENCHMARKS_BENCHMARKCOMMON_H_
#define MATHFU_BENCHMARKS_BENCHMARKCOMMON_H_

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#endif  // defined(_WIN32)

// This macro stands in for a double loop over a vector with size elements.
// This double loop is repeated iterations times to create more accurate
// performance test.
#define PERFTEST_2D_VECTOR_LOOP(iterations, size) \
  for (unsigned int k = 0; k < iterations; k++) \
  for (unsigned int i = 0; i < size; i++) \
  for (unsigned int j = 0; j < size; j++)

// High resolution timer.
class Timer {
 public:
  Timer() {
    InitializeTickPeriod();
    Reset();
  }

  // Save the current number of counter ticks.
  void Reset() {
    start_ = GetTicks();
  }

  // Get the time elapsed in counter ticks since Reset() was called.
  unsigned long long GetElapsedTicks() {
    return GetTicks() - start_;
  }

  // Get the time elapsed in seconds since Reset() was called.
  double GetElapsedSeconds() {
    return static_cast<double>(GetElapsedTicks()) * tick_period();
  }

 public:
  // Initialize the tick period value.
  static void InitializeTickPeriod() {
    if (tick_period_ != 0) {
      return;
    }
#if defined(_WIN32)
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    tick_period_ = 1.0 / static_cast<double>(frequency.QuadPart);
#elif defined(__linux__)
    // Use a fixed frequency of 1ns to match timespec.
    tick_period_ = 1e-9;
#else
    // Use a fixed frequency of 1us to match timeval.
    tick_period_ = 1e-6;
#endif // defined(_WIN32)
  }

  // Get the period of one counter tick.
  static double tick_period() {
    return tick_period_;
  }

  // Get the number of counter ticks elapsed.
  static unsigned long long GetTicks() {
#if defined(_WIN32)
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return ticks.QuadPart;
#elif defined(__linux__)
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (static_cast<unsigned long long>(time.tv_sec) * 1000000000ULL) +
        time.tv_nsec;
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    return (static_cast<unsigned long long>(time.tv_sec) * 1000000ULL) +
        time.tv_usec;
#endif
  }

 private:
  unsigned long long start_;
  static double tick_period_;
};
double Timer::tick_period_ = 0;

#endif  // MATHFU_BENCHMARKS_BENCHMARKCOMMON_H_
