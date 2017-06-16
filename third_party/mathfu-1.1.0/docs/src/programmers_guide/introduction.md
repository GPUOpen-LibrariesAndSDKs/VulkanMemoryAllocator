Introduction    {#mathfu_guide_introduction}
============

# About MathFu    {#mathfu_guide_about_mathfu}

[MathFu][] is a C++ math library developed primarily for games focused on
simplicity and efficiency.

It provides a suite of [vector][], [matrix][] and [quaternion][] classes
to perform basic [geometry][] suitable for game developers.  This functionality
can be used to construct geometry for graphics libraries like [OpenGL][] or
perform calculations for animation or physics systems.

# Prerequisites    {#mathfu_guide_prerequisites}

[MathFu][] is written in C++, you are expected to be experienced in C++
programming. [MathFu][] should not be your first C++ programming project! You
should be comfortable with compiling, linking, and debugging.

# About This Guide    {#mathfu_guide_about_guide}

This guide provides an overview of the [MathFu] API, it does *not* cover
every aspect of functionality provided by the library.  The entire API
is documented by the [API reference][].  In addition, the unit tests
(under `mathfu/unit_tests`) provide example usage of each class and function
in the library.

# Concepts    {#mathfu_guide_concepts}

The core functionality of [MathFu][] is provided by the following classes:
   * [Vector](@ref mathfu::Vector)
   * [Matrix](@ref mathfu::Matrix)
   * [Quaternion](@ref mathfu::Quaternion)

Each class is described in the following sections of the guide:
   * [Vector](@ref mathfu_guide_vectors)
      - A geometric concept with a magnitude and direction, defined in any
        dimensional space.
   * [Matrix](@ref mathfu_guide_matrices)
      - A set of data organized in rows and columns.
        [MathFu][] matricies may have any number of rows and columns.
   * [Quaternion](@ref mathfu_guide_quaternions)
      - A specific type of four dimensional vector and defines a rotation in
        three dimensional space.

In addition, [MathFu][] provides a suite of general math functions and method
that make it easier to handle the special requirements of [SIMD][] data types
described in the [Utilities](@ref mathfu_guide_utilities) section.

# Optimization    {#mathfu_guide_optimization}

[MathFu][] is optimized using [SIMD][] instructions (including [NEON][] for
[ARM][] and [SSE][] for [x86][] architectures).  [SIMD][] optimization is
enabled by default based upon the target platform and compiler options used
to build a project.  In addition, [MathFu][] provides compile time options to
modify code generation, see [Build Configurations][] for more details.

## Supporting Additional Architectures

[MathFu][]'s [SIMD][] implementation uses [vectorial][] as an abstraction
layer for common [SIMD][] instructions and data types.  To support additional
architectures, contributors can add support for new [SIMD][] instructions and
data types to the [vectorial][] project and then modify the code in
mathfu/utilities.h to define the macro <code>MATHFU_COMPILE_WITH_SIMD</code>
for the new architecture.

  [ARM]: http://en.wikipedia.org/wiki/ARM_architecture
  [API reference]: @ref mathfu_api_reference
  [matrix]: http://en.wikipedia.org/wiki/Matrix_(mathematics)
  [geometry]: http://en.wikipedia.org/wiki/Geometry
  [vector]: http://en.wikipedia.org/wiki/Euclidean_vector
  [quaternion]: http://en.wikipedia.org/wiki/Quaternion
  [OpenGL]: http://www.opengl.org/
  [MathFu]: @ref mathfu_overview
  [vectorial]: http://github.com/scoopr/vectorial
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [SSE]: http://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
  [x86]: http://en.wikipedia.org/wiki/X86
  [NEON]: http://www.arm.com/products/processors/technologies/neon.php
  [Build Configurations]: @ref mathfu_build_config