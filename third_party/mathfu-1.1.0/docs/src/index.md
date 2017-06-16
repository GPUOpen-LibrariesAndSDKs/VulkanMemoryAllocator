MathFu    {#mathfu_index}
======

# Overview    {#mathfu_overview}

[MathFu][] is a C++ math library developed primarily for games focused on
simplicity and efficiency.

It provides a suite of [vector][], [matrix][] and [quaternion][] classes
to perform basic [geometry][] suitable for game developers.  This functionality
can be used to construct geometry for graphics libraries like [OpenGL][] or
perform calculations for animation or physics systems.

[MathFu] can be downloaded from [GitHub](http://github.com/google/mathfu) or
the [releases page](http://github.com/google/mathfu/releases).

**Important**: MathFu uses submodules to reference other components it depends
upon so download the source using:

~~~{.sh}
    git clone --recursive https://github.com/google/mathfu.git
~~~

# Functionality

Each [vector][], [matrix][] and [quaternion][] class implements basic
arithmetic operators (addition, subtraction, multiplication, division) in
addition to simple [linear algebra][] operators (e.g dot product,
cross product, inverse, slerp).

# Optimization

[MathFu][] is optimized using [SIMD][] instructions (including [NEON][] for
[ARM][] and [SSE][] for [x86][] architectures).  [SIMD][] optimization is
enabled by default based upon the target platform and compiler options used
to build a project.  In addition, [MathFu][] provides compile time options to
modify code generation, see [Build Configurations][] for more details.

# Supported Platforms

[MathFu][] has been tested on the following platforms:

   * [Android][]
   * [Linux][] (x86_64)
   * [OS X][]
   * [Windows][]

This library is entirely written in C++ with the exception of some
conditionally enabled compiler intrinsics and some workarounds for different
compiler quirks, which means that it *should* work with either no or little
modification on any platform that has a C++ compiler.

# Feedback and Reporting Bugs

   * Discuss MathFu with other developers and users on the
     [MathFu Google Group][].
   * File issues on the [MathFu Issues Tracker][].
   * Post your questions to [stackoverflow.com][] with a mention of **mathfu**.

  [Android]: http://www.android.com
  [ARM]: http://en.wikipedia.org/wiki/ARM_architecture
  [Build Configurations]: @ref mathfu_build_config
  [Linux]: http://en.m.wikipedia.org/wiki/Linux
  [MathFu Google Group]: http://groups.google.com/group/mathfulib
  [MathFu Issues Tracker]: http://github.com/google/mathfu/issues
  [MathFu]: @ref mathfu_overview
  [NEON]: http://www.arm.com/products/processors/technologies/neon.php
  [OS X]: http://www.apple.com/osx/
  [OpenGL]: http://www.opengl.org/
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [SSE]: http://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
  [Windows]: http://windows.microsoft.com/
  [geometry]: http://en.wikipedia.org/wiki/Geometry
  [linear algebra]: http://en.wikipedia.org/wiki/Linear_algebra
  [matrix]: http://en.wikipedia.org/wiki/Matrix_(mathematics)
  [quaternion]: http://en.wikipedia.org/wiki/Quaternion
  [stackoverflow.com]: http://stackoverflow.com/search?q=mathfu
  [vector]: http://en.wikipedia.org/wiki/Euclidean_vector
  [x86]: http://en.wikipedia.org/wiki/X86
