Building    {#mathfu_guide_building}
========

# Integration Options    {#mathfu_guide_building_options}

[MathFu][] is designed as a C++ header only library to simplify the process
of including the library in projects.  [MathFu] provides the following options
for using it in a project:

   * [Manually configure the compiler][] by simply include the appropriate
     headers into a project's source code.
   * Use the [CMake project][].
   * Include the [Android NDK project][] when developing for [Android][]

Before diving into using [MathFu][] in your project, we recommend you spend
some time building and running the unit tests for each platform you wish to
target by working through the following sections:

   * [Building for Android][]
   * [Building for Linux][]
   * [Building for OS X][]
   * [Building for Windows][]

# Manual Compiler Configuration   {#mathfu_guide_building_compiler_config}

The following directories need to be added to the include paths of a project
to build the default configuration:
   * `mathfu/include`
   * `vectorial/include`
      - If [vectorial][] was downloaded with the [MathFu][] project, this
        will be located under the `dependencies/` directory.

In addition, to compile with [SIMD][] enabled an architecture specific flag
must be specified:

Architecture    | Compiler            | Flag
----------------|---------------------|------------
[x86][]         | Android GCC / Clang | -msse
[x86_64][]      | Android GCC / Clang | -msse4.1
[x86][]         | GCC / Clang         | -msse4.1
[ARM][]         | GCC / Clang         | -mfpu=neon
[x86][]         | Visual Studio       | /arch:SSE2

For example, to compile the following file `test.cpp` against [MathFu][] with
gcc for x86:

~~~{.cpp}
#include <stdio.h>
#include "mathfu/vector.h"

int main(int argc, char *argv[]) {
  mathfu::Vector<float, 3> v1(1.0f, 2.0f, 3.0f), v2(3.0f, 2.5f, 0.5f), v3;
  v3 = v1 + v2;
  printf("%.1f %.1f %.1f\n", v3.x(), v3.y(), v3.z());
  return 0;
}
~~~

requires the following command line (assuming vectorial is in the
`mathfu/dependencies` directory):

~~~{.sh}
g++ -Imathfu/include -Imathfu/dependencies/vectorial/include test.cpp -msse4.1
~~~

In addition, [MathFu][] provides a set of build configuration options that
are controlled using preprocessor symbols described by [Build Configuration][].

# CMake    {#mathfu_guide_building_cmake}

[MathFu][] uses a [CMake][] project to build unit tests and benchmarks
for [Linux][], [OS X][] and [Windows][].

[CMake][] is used to generate a platform projects for each target platform:
  * [Makefiles][] ([Linux][] / [OS X][])
  * [Xcode][] projects ([OS X][])
  * [Visual Studio][] projects ([Windows][])

If you're not familiar with [CMake][], see the following sections to learn how
to build on each platform:

   * [Building for Linux][]
   * [Building for OS X][]
   * [Building for Windows][]

In addition to building the [MathFu][] unit tests and benchmarks,
the [MathFu][] [CMake][] project can be used in developers own projects by:

   * Disabling the build of unit tests and benchmarks using the options:
      - `mathfu_build_benchmarks`
      - `mathfu_build_tests`
   * Including the [CMake][] project.
   * Using the `mathfu_configure_flags` function to add the appropriate
     include directories and compiler flags for the project.

For example, in a CMakeLists.txt project file which contains the executable
`mygame`:

~~~
# Include MathFu in this project with test and benchmark builds disabled
# then add MathFu compiler flags to the mygame build target.
set(mathfu_build_benchmarks OFF CACHE BOOL "")
set(mathfu_build_tests OFF CACHE BOOL "")
add_subdirectory(path/to/mathfu mathfu)

mathfu_configure_flags(mygame)
~~~

[MathFu][] build options (see [Build Configuration][]) can be configured
with the `mathfu_configure_flags` function using the optional arguments
`enable_simd` and `force_padding`.  For example:

Function Call                               | Build Configuration
--------------------------------------------|----------------------------------
`mathfu_configure_flags(mygame)`            | Default, SIMD & padding enabled.
`mathfu_configure_flags(mygame TRUE FALSE)` | SIMD enabled & padding disabled.
`mathfu_configure_flags(mygame FALSE)`      | SIMD disabled.

See the function comment in the CMakeLists.txt file for more information.

# Android NDK Makefiles    {#mathfu_guide_building_android_makefiles}

If you're not familiar developing applications with the [Android NDK][],
see [Building for Android][] first.

To use [MathFu][] with an [Android NDK][] makefile project, add the following
lines to your project's `Android.mk` file.

~~~{.mk}
# Add the empty MathFu static library target to configure the project.
LOCAL_STATIC_LIBRARIES += libmathfu

# This is used to build your project's shared library, this should already
# be in your Android.mk makefile.
include $(BUILD_SHARED_LIBRARY)

# Add the directory containing MathFu to the module search path.
$(call import-add-path,$(abspath path/to/mathfu/..))
# Import the MathFu project.
$(call import-module,mathfu/jni)
~~~

[MathFu][] build options (see [Build Configuration][]) can be configured
by linking against the different static libraries the [MathFu][] project
builds:

Library                | Build Configuration
-----------------------|--------------------------------------------------
`libmathfu`            | Default configuration, SIMD and padding enabled.
`libmathfu_no_padding` | SIMD enabled (if supported), padding disabled.
`libmathfu_no_simd`    | SIMD disabled, padding disabled.

<br>

  [Android NDK project]: @ref mathfu_guide_building_android_makefiles
  [Android NDK]: http://developer.android.com/tools/sdk/ndk/index.html
  [Android]: http://www.android.com
  [ARM]: http://en.wikipedia.org/wiki/ARM_architecture
  [Build Configuration]: @ref mathfu_build_config
  [Building for Android]: @ref mathfu_guide_building_android
  [Building for Linux]: @ref mathfu_guide_building_linux
  [Building for OS X]: @ref mathfu_guide_building_osx
  [Building for Windows]: @ref mathfu_guide_building_windows
  [CMake project]: @ref mathfu_guide_building_cmake
  [CMake]: http://www.cmake.org/
  [Linux]: http://en.m.wikipedia.org/wiki/Linux
  [Makefiles]: http://www.gnu.org/software/make/
  [Manually configure the compiler]: @ref mathfu_guide_building_compiler_config
  [MathFu]: @ref mathfu_overview
  [OS X]: http://www.apple.com/osx/
  [SIMD]: http://en.wikipedia.org/wiki/SIMD
  [Visual Studio]: http://www.visualstudio.com/
  [Windows]: http://windows.microsoft.com/
  [Xcode]: http://developer.apple.com/xcode/
  [vectorial]: http://github.com/scoopr/vectorial
  [x86]: http://en.wikipedia.org/wiki/X86
  [x86_64]: http://en.wikipedia.org/wiki/X86
