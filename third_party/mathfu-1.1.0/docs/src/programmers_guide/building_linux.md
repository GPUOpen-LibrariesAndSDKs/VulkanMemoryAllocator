Building for Linux    {#mathfu_guide_building_linux}
==================

# Version Requirements    {#mathfu_guide_building_linux_version}

Following are the minimum required versions of tools and libraries you
need to build [MathFu][] for Android:

   * [CMake][]: 2.8.12.1

# Prerequisites    {#mathfu_guide_building_linux_prerequisites}

Prior to building, install the following components using the [Linux][]
distribution's package manager:

   * [CMake][].  You can also manually install packages from
     [cmake.org](http://cmake.org).

For example, on [Ubuntu][]:

~~~{.sh}
    sudo apt-get install cmake
~~~

# Building    {#mathfu_guide_building_linux_building}

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Generate [Makefiles][] from the [CMake][] project.
   * Execute `make` to build the unit tests and benchmarks.

For example:

~~~{.sh}
    cd mathfu
    cmake -G'Unix Makefiles' .
    make
~~~

To perform a debug build:

~~~{.sh}
    cd mathfu
    cmake -G'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug .
    make
~~~

Build targets can be configured using options exposed in
`mathfu/CMakeLists.txt` by using [CMake]'s `-D` option.
Build configuration set using the `-D` option is sticky across subsequent
builds.

For example, if a build is performed using:

~~~{.sh}
    cd mathfu
    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .
    make
~~~

to switch to a release build CMAKE_BUILD_TYPE must be explicitly specified:

~~~{.sh}
    cd mathfu
    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .
    make
~~~

# Running Applications    {#mathfu_guide_building_linux_running}

Each benchmark binaries are located in the `benchmarks/` directory and
unit test binaries are located in the `unit_tests/` directory.

To run all benchmarks:

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Run each benchmark binary in the `benchmarks/` directory.

For example:

~~~{.sh}
    cd mathfu
    for binary in ./benchmarks/*_benchmarks; do ${binary}; done
~~~

To run all unit tests:

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Run each unit test binary in the `unit_tests/` directory.

For example:

~~~{.sh}
    cd mathfu
    for binary in ./unit_tests/*_tests; do ${binary}; done
~~~

<br>

  [CMake]: http://www.cmake.org/
  [Linux]: http://en.wikipedia.org/wiki/Linux
  [Makefiles]: http://www.gnu.org/software/make/
  [MathFu]: @ref mathfu_overview
  [Ubuntu]: http://www.ubuntu.com
