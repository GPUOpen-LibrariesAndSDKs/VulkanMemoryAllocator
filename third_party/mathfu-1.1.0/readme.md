MathFu Version 1.1.0    {#mathfu_readme}
====================

MathFu is a C++ math library developed primarily for games focused on
simplicity and efficiency.

It provides a suite of [vector][], [matrix][] and [quaternion][] classes
to perform basic [geometry][] suitable for game developers.  This functionality
can be used to construct geometry for graphics libraries like [OpenGL][] or
perform calculations for animation or physics systems.

The library is written in portable C++ with [SIMD][] compiler intrinsics and
has been tested on the following platforms:

   * [Android][]
   * [Linux][] (x86_64)
   * [OS X][]
   * [Windows][]

Go to our [landing page][] to browse our documentation and see some examples.

   * Discuss MathFu with other developers and users on the
     [MathFu Google Group][].
   * File issues on the [MathFu Issues Tracker][]
   * Post your questions to [stackoverflow.com][] with a mention of
     **mathfu**.

**Important**: MathFu uses submodules to reference other components it depends
upon so download the source using:

    git clone --recursive https://github.com/google/mathfu.git

To contribute to this project see [CONTRIBUTING][].

For applications on Google Play that integrate this tool, usage is tracked.
This tracking is done automatically using the embedded version string
(kMathFuVersionString), and helps us continue to optimize it. Aside from
consuming a few extra bytes in your application binary, it shouldn't affect
your application at all. We use this information to let us know if MathFu
is useful and if we should continue to invest in it. Since this is open
source, you are free to remove the version string but we would appreciate if
you would leave it in.

  [Android]: http://www.android.com
  [Linux]: http://en.m.wikipedia.org/wiki/Linux
  [MathFu Google Group]: http://groups.google.com/group/mathfulib
  [MathFu Issues Tracker]: http://github.com/google/mathfu/issues
  [OS X]: http://www.apple.com/osx/
  [OpenGL]: http://www.opengl.org/
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [Windows]: http://windows.microsoft.com/
  [geometry]: http://en.wikipedia.org/wiki/Geometry
  [landing page]: http://google.github.io/mathfu
  [matrix]: http://en.wikipedia.org/wiki/Matrix_(mathematics)
  [quaternion]: http://en.wikipedia.org/wiki/Quaternion
  [stackoverflow.com]: http://stackoverflow.com/search?q=mathfu
  [vector]: http://en.wikipedia.org/wiki/Euclidean_vector
  [CONTRIBUTING]: http://github.com/google/mathfu/blob/master/CONTRIBUTING
