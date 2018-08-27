-- _ACTION is a premake global variable and for our usage will be vs2012, vs2013, etc.
-- Strip "vs" from this string to make a suffix for solution and project files.
_SUFFIX = _ACTION

workspace "VulkanSample"
configurations { "Debug", "Release" }
platforms { "x64", "Linux-x64" }
location "../build"
filename ("VulkanSample_" .. _SUFFIX)
startproject "VulkanSample"

filter "platforms:x64"
system "Windows"
architecture "x64"
includedirs { "$(VULKAN_SDK)/Include" }
libdirs { "$(VULKAN_SDK)/Lib" }

filter "platforms:Linux-x64"
system "Linux"
architecture "x64"
includedirs { "$(VULKAN_SDK)/include" }
libdirs { "$(VULKAN_SDK)/lib" }


project "VulkanSample"
kind "ConsoleApp"
language "C++"
location "../build"
filename ("VulkanSample_" .. _SUFFIX)
targetdir "../bin"
objdir "../build/Desktop_%{_SUFFIX}/%{cfg.platform}/%{cfg.buildcfg}"
floatingpoint "Fast"
files { "../src/*.h", "../src/*.cpp" }
flags { "NoPCH", "FatalWarnings" }
characterset "Unicode"

filter "configurations:Debug"
defines { "_DEBUG", "DEBUG" }
flags { }
targetsuffix ("_Debug_" .. _SUFFIX)

filter "configurations:Release"
defines { "NDEBUG" }
optimize "On"
flags { "LinkTimeOptimization" }
targetsuffix ("_Release_" .. _SUFFIX)

filter { "platforms:x64" }
defines { "WIN32", "_CONSOLE", "PROFILE", "_WINDOWS", "_WIN32_WINNT=0x0601" }
links { "vulkan-1" }

filter { "platforms:Linux-x64" }
buildoptions { "-std=c++0x" }
links { "vulkan" }

filter { "configurations:Debug", "platforms:x64" }
buildoptions { "/MDd" }

filter { "configurations:Release", "platforms:Windows-x64" }
buildoptions { "/MD" }


project "VmaReplay"
removeplatforms { "Linux-x64" }
kind "ConsoleApp"
language "C++"
location "../build"
filename ("VmaReplay_" .. _SUFFIX)
targetdir "../bin"
objdir "../build/Desktop_%{_SUFFIX}/%{cfg.platform}/%{cfg.buildcfg}"
floatingpoint "Fast"
files { "../src/VmaReplay/*.h", "../src/VmaReplay/*.cpp" }
flags { "NoPCH", "FatalWarnings" }
characterset "Default"

filter "configurations:Debug"
defines { "_DEBUG", "DEBUG" }
flags { }
targetsuffix ("_Debug_" .. _SUFFIX)

filter "configurations:Release"
defines { "NDEBUG" }
optimize "On"
flags { "LinkTimeOptimization" }
targetsuffix ("_Release_" .. _SUFFIX)

filter { "platforms:x64" }
defines { "WIN32", "_CONSOLE", "PROFILE", "_WINDOWS", "_WIN32_WINNT=0x0601" }
links { "vulkan-1" }

filter { "platforms:Linux-x64" }
buildoptions { "-std=c++0x" }
links { "vulkan" }

filter { "configurations:Debug", "platforms:x64" }
buildoptions { "/MDd" }

filter { "configurations:Release", "platforms:Windows-x64" }
buildoptions { "/MD" }
