Building for Windows    {#mathfu_guide_building_windows}
====================

# Version Requirements    {#mathfu_guide_building_windows_version}

Following are the minimum required versions of tools and libraries you
need to build [MathFu][] for Android:

   * [Windows][]: 7
   * [Visual Studio][]: 2012
   * [CMake][]: 2.8.12.1

# Prerequisites    {#mathfu_guide_building_windows_prerequisites}

Prior to building, install the following:

   * [Visual Studio][]
   * [CMake][] from [cmake.org](http://cmake.org).

# Building with Visual Studio    {#mathfu_guide_building_windows_building}

Generate the [Visual Studio][] project using [CMake][]:

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Use [CMake][] to generate the [Visual Studio][] project.

~~~{.sh}
    cd mathfu
    cmake -G "Visual Studio 11 2012" .
~~~

Open the [MathFu][] solution in [Visual Studio][].

   * Double-click on `mathfu/MathFu.sln` to open the solution.
   * Select "Build-->Build Solution" from the menu.

# Running Applications    {#mathfu_guide_building_windows_running}

## Running in Visual Studio    {#mathfu_guide_building_osx_run_vs}

Open [Visual Studio][] select a build target and run it:

   * Double-click on `mathfu/MathFu.sln` to open the solution in
     [Visual Studio][].
   * Right click on a project, for example `matrix_tests`, select
     "Set as StartUp Project" from the menu.
   * Select "Debug-->Start Debugging" from the menu.

## Running from the Command Line {#mathfu_guide_building_windows_cmdline}

To run all benchmarks:

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Run each benchmark binary in the `benchmarks/` directory.

For example:

~~~{.sh}
    cd mathfu
    for %a in (benchmarks\Debug\*.exe) do %a
~~~

To run all unit tests:

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Run each unit test binary in the `unit_tests/` directory.

For example:

~~~{.sh}
    cd mathfu
    for %a in (unit_tests\Debug\*.exe) do %a
~~~

<br>

  [CMake]: http://www.cmake.org
  [MathFu]: @ref mathfu_overview
  [Visual Studio]: http://www.visualstudio.com/
  [Windows]: http://windows.microsoft.com/

