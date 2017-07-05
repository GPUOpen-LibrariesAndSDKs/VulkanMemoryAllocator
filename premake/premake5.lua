-- _ACTION is a premake global variable and for our usage will be vs2012, vs2013, etc.
-- Strip "vs" from this string to make a suffix for solution and project files.
_SUFFIX = string.gsub(_ACTION, "vs", "")

workspace "VulkanSample"
configurations { "Debug", "Release" }
platforms { "x64" }
location "../build"
filename ("VulkanSample_" .. _SUFFIX)
startproject "VulkanSample"

filter "platforms:x64"
system "Windows"
architecture "x64"
includedirs { "../third_party/mathfu-1.1.0/include", "$(VULKAN_SDK)/Include" }
libdirs { "$(VULKAN_SDK)/Lib" }

project "VulkanSample"
kind "ConsoleApp"
language "C++"
location "../build"
filename ("VulkanSample_" .. _SUFFIX)
targetdir "../bin"
objdir "../build/Desktop_%{_SUFFIX}/%{cfg.platform}/%{cfg.buildcfg}"
floatingpoint "Fast"
files { "../src/*.h", "../src/*.cpp" }
links { "vulkan-1" }
flags { "NoPCH", "FatalWarnings" }
characterset "Unicode"
defines { "WIN32", "_CONSOLE", "PROFILE", "_WINDOWS", "_WIN32_WINNT=0x0601" }

filter "configurations:Debug"
defines { "_DEBUG", "DEBUG" }
flags { }
targetsuffix ("_Debug_" .. _SUFFIX)
buildoptions { "/MDd" }

filter "configurations:Release"
defines { "NDEBUG" }
optimize "On"
flags { "LinkTimeOptimization" }
targetsuffix ("_Release_" .. _SUFFIX)
buildoptions { "/MD" }
