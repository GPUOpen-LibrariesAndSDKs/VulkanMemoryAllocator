Building for Android    {#mathfu_guide_building_android}
====================

# Version Requirements    {#mathfu_guide_building_android_version}

Following are the minimum tested versions of the tools and libraries you
need to build [MathFu][] for Android:

   * [Android SDK][]:  Android 2.3.3 (API Level 10) or above.
   * [Android Developer Tools][] ([ADT][]): 20140702
      - NDK plugn for Eclipse (bundled with [ADT][]) if using Eclipse to build.
   * [Android NDK][]: android-ndk-r10e
   * [fplutil][]: 1.0 or above, if using [fplutil][] to build, install and run.

# Prerequisites    {#mathfu_guide_building_android_prerequisites}

Prior to building:

   * Install the [Android SDK][].
   * Install the [Android NDK][].
   * Install [fplutil prerequisites][] if using [fplutil][] to build, install
     and run.

# Building    {#mathfu_guide_building_android_building}

Each [MathFu][] project for Android has an associated `AndroidManifest.xml`
file and `jni` subdirectory.  Unit tests and benchmarks directories contain
projects that each build an Android package ([apk][]) which can be installed
and executed on Android devices.

The following directories in the [MathFu] project contain [Android NDK][]
projects:

   * `mathfu/`
      - Rules used to build the [MathFu][] library.
   * `mathfu/benchmarks`
      - Each subdirectory under this directory contains a simple benchmark
        used to measure the performance of different build configurations.
   * `mathfu/unit_tests`
      - Each subdirectory under this directory contains a unit test application
        used to test different components of the library in different build
        configurations.

## Building with ndk-build  {#mathfu_guide_building_android_building_ndk_build}

To build a single [Android NDK][] project (without packaging the native
component in an [apk][]):

   * Open a command line window.
   * Go to the directory containing the project to build.
   * `ndk-build`

For example, to build the matrix test for the default build configuration
without generating an [apk][]:

~~~{.sh}
    cd mathfu/unit_tests/matrix_test/default
    ndk-build
~~~

## Building with fplutil    {#mathfu_guide_building_android_building_fplutil}

To build all [Android NDK][] projects, install and run them on a device:

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Execute `dependencies/fplutil/bin/build_all_android.py`

For example:

~~~{.sh}
    cd mathfu
    ./dependencies/fplutil/bin/build_all_android.py
~~~

# Installing and Running Applications {#mathfu_guide_building_android_running}

## Using Eclipse {#mathfu_guide_building_android_eclipse}

Running a sample requires the [Android Developer Tools][] ([ADT][]) plugin and
the [NDK Eclipse plugin][].

   * Build a project using `ndk-build`
     ([see above](@ref mathfu_guide_building_android_building_ndk_build)).
   * Open [ADT][] Eclipse.
   * Select "File->Import..." from the menu.
   * Select "Android > Existing Android Code Into Workspace", and click "Next".
   * Click the "Browse..." button next to `Root Directory:` and select the
     project folder (e.g. `mathfu/unit_tests/matrix_test/default`).
   * Click "Finish". Eclipse imports the project, and displays it in the
     Package Explorer pane.
   * Right-click the project, and select "Run->Run As->Android Application"
      from the menu.
   * If you do not have a physical device, you must define a virtual one.
     For details about how to define a virtual device, see [managing avds][].
     We don’t recommend a virtual device for development.
   * None of the applications have a visual component so their output is
     visible via the log ([adb][] logcat).

## Using fplutil {#mathfu_guide_building_android_fplutil}

To install and run a single application:

   * Open a command line window.
   * Go to the directory containing the project to install and run.
   * Execute `dependencies/fplutil/bin/build_all_android.py` with the `-i`
     and `-r` options.

For example:

~~~{.sh}
    cd mathfu/unit_tests/matrix_test/default
    ./dependencies/fplutil/bin/build_all_android.py -i -r
~~~

To install and run all applications:

   * Open a command line window.
   * Go to the [MathFu][] project directory.
   * Execute `dependencies/fplutil/bin/build_all_android.py` with the `-i`
     and `-r` options.

For example:

~~~{.sh}
    cd mathfu
    ./dependencies/fplutil/bin/build_all_android.py -i -r
~~~

# Code Generation    {#mathfu_guide_building_android_code_generation}

By default, code is generated for devices that support the `armeabi-v7a`
ABI.  Alternatively, you can generate a fat `.apk` that includes code for all
ABIs.  To do so, override APP\_ABI on ndk-build's command line.

Using `ndk-build`:

~~~{.sh}
    ndk-build APP_ABI=all
~~~

Using `fplutil`:
~~~{.sh}
    ./dependencies/fplutil/bin/build_all_android.py -f APP_ABI=all
~~~

<br>

  [MathFu]: @ref mathfu_overview
  [adb]: http://developer.android.com/tools/help/adb.html
  [ADT]: http://developer.android.com/tools/sdk/eclipse-adt.html
  [Android Developer Tools]: http://developer.android.com/sdk/index.html
  [Android NDK]: http://developer.android.com/tools/sdk/ndk/index.html
  [Android SDK]: http://developer.android.com/sdk/index.html
  [NDK Eclipse plugin]: http://developer.android.com/sdk/index.html
  [apk]: http://en.wikipedia.org/wiki/Android_application_package
  [fplutil]: http://google.github.io/fplutil
  [fplutil prerequisites]: http://google.github.io/fplutil/fplutil_prerequisites.html
  [managing avds]: http://developer.android.com/tools/devices/managing-avds.html
