Utilities    {#mathfu_guide_utilities}
=========

# Memory Allocation    {#mathfu_guide_utilities_allocation}

[MathFu][]'s use of [SIMD][] instructions provides significant performance
gains in many use cases.  However, some architectures have memory
alignment requirements for data used by [SIMD][] operations (e.g [SIMD][]
data structures must be 16-byte aligned on x86).  Some STL implementations do
not respect data type alignment which makes it more difficult to respect the
[SIMD][] memory alignment requirements.

In order to adhere to [SIMD][] alignment requirements, [MathFu][] provides a
dynamic memory allocator [AllocateAligned()][] which ensures data is correctly
aligned.  Memory allocated by [AllocateAligned()][] **must** be deallocated
using [FreeAligned()][], for example:

~~~{.cpp}
   void *memory = mathfu::AllocateAligned(32);
   mathfu::FreeAligned(memory);
~~~

The [simd_allocator](@ref mathfu::simd_allocator) class is provided to perform
aligned memory allocation with STL classes like std::vector.  For example, the
following constructs a vector of `vec4` which uses the aligned memory
allocator:

~~~{.cpp}
    std::vector<vec4, mathfu::simd_allocator<mathfu::vec4>> myvector;
~~~

The aligned memory allocator uses [MATHFU_ALIGNMENT](@ref MATHFU_ALIGNMENT)
bytes of additional memory per allocation.  If the additional memory usage
per allocation is acceptable for an application, the most simple solution
is to override the global `new` and `delete` operators by adding
[MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE](@ref MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE)
to the source file containing an application's entry point (e.g `main.cpp`):

~~~{.cpp}
#include "mathfu/utilities.h"

MATHFU_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE

int main(int argc, char *argv[]) {
  // The application.
  return 0;
}
~~~

# Miscellaneous Functions    {#mathfu_guide_utilities_misc}

[Clamp](@ref mathfu_Clamp) is used to clamp a value within a specified
range.  For example:

~~~{.cpp}
    float x = 1.0f;
    z = mathfu::Clamp(x, 1.0f, 2.0f);  // z = 1.0
    z = mathfu::Clamp(x, 1.5f, 2.0f);  // z = 1.5
    z = mathfu::Clamp(x, 0.0f, 0.5f);  // z = 0.5
~~~

[Lerp](@ref mathfu_Lerp) linearly interpolates between two values of an
arbitrary type in a specified range.  For example, an object whose position is
specified by a [Vector][] could be moved over a range as time elapses:

~~~{.cpp}
    const mathfu::vec2 start(2.0f, 3.0f);
    const mathfu::vec2 end(10.0f, 0.0f);
    // Update the position of an object every 100ms.
    for (float time = 0.0f; time < 1.1f; time += 0.1f) {
      mathfu::vec2 position = mathfu::Lerp(start, end, time);
      // Draw object.
      // Wait 100ms.
    }
~~~

## Random Number Generation    {#mathfu_guide_utilities_random}

[Random()][] generates pseudo random floating point numbers using `rand()`
from the C standard library.  For example, the following generates 3 random
numbers:

~~~{.cpp}
    float value1, value2, value2;
    value1 = mathfu::Random<float>();
    value2 = mathfu::Random<float>();
    value3 = mathfu::Random<float>();
~~~

[Random()][] is used by [RandomRange()](@ref mathfu_RandomRange) to generate
a random number within a symmetric range.  For example, to generate a random
value between -10.0f..10.0f:

~~~{.cpp}
    float random_value = mathfu::RandomRange(10.0f);
~~~

Finally, [RandomInRange](@ref mathfu_RandomInRange) can be used to generate
a random number within an arbitrary range.  For example, to generate a random
value between 5.0f..7.0f:

~~~{.cpp}
    float random_value = mathfu::RandomInRange(5.0f, 7.0f);
~~~

<br>

  [AllocateAligned()]: @ref mathfu_AllocateAligned
  [FreeAligned()]: @ref mathfu_FreeAligned
  [MathFu]: @ref mathfu_overview
  [Random()]: @ref mathfu_Random
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [Vector]: @ref mathfu::Vector
