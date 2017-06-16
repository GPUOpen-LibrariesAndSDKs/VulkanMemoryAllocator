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
#include "mathfu/vector.h"
#include "mathfu/utilities.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "benchmark_common.h"

// Number of elements to iterate over.
static const size_t kVectorSize = 1000;
// Number of iterations of each operation.
static const size_t kIterations = 100;

using mathfu::Random;
using mathfu::Vector;

// This test creates a number of vectors and performs some mathematical
// operations on them in order to measure expected performance of vector
// operations.
int main(int argc, char** argv) {
  typedef float T;
  (void)argc;
  (void)argv;
  // Create a array of vectors
  Vector<T, 3> *vectors = new Vector<T, 3>[kVectorSize];
  T final_sum = 0;
  Vector<T, 3> sum(0.0f);
  for (size_t i = 0; i < kVectorSize; i++) {
    Vector<T, 3> vec(Random<T>(), Random<T>(), Random<T>());
    if (vec.LengthSquared() == static_cast<T>(0.0)) {
      vec.x = static_cast<T>(1.0);
    }
    vectors[i] = vec;
  }
  printf("Running vector benchmark (%s)...\n", MATHFU_BUILD_OPTIONS_STRING);
  // Start vector performance code. Run a number of loops for more accurate
  // numbers.
  Timer timer;
  PERFTEST_2D_VECTOR_LOOP(kIterations, kVectorSize) sum += vectors[j];
  PERFTEST_2D_VECTOR_LOOP(kIterations, kVectorSize) sum -= vectors[j];
  PERFTEST_2D_VECTOR_LOOP(kIterations, kVectorSize) sum *= 0.1f;
  PERFTEST_2D_VECTOR_LOOP(kIterations, kVectorSize) {
    sum += Vector<T, 3>::CrossProduct(vectors[i], vectors[j]);
  }
  PERFTEST_2D_VECTOR_LOOP(kIterations, kVectorSize) {
    final_sum += Vector<T, 3>::DotProduct(vectors[j], vectors[i]);
  }
  PERFTEST_2D_VECTOR_LOOP(kIterations, kVectorSize) {
      final_sum -= vectors[i].Length();
  }
  PERFTEST_2D_VECTOR_LOOP(kIterations, kVectorSize) {
    final_sum += vectors[i].Normalize();
  }
  final_sum += sum[0] + sum[1] + sum[2];
  // End vector performance code
  double elapsed = timer.GetElapsedSeconds();
  printf("Took %f seconds\n", elapsed);
  delete [] vectors;
  return 0;
}
