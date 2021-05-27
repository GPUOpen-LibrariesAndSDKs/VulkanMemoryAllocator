cmake_minimum_required(VERSION 3.9)

project(VulkanMemoryAllocator)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

find_package(Vulkan REQUIRED)

# Put binaries into bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# VulkanMemoryAllocator contains an example application and VmaReplay which are not build by default
option(VMA_BUILD_EXAMPLE_APP "Build VulkanMemoryAllocator example application" OFF)
option(VMA_BUILD_EXAMPLE_APP_SHADERS "Build VulkanMemoryAllocator example application's shaders" OFF)
option(VMA_BUILD_REPLAY_APP "Build VulkanMemoryAllocator replay application" OFF)

add_subdirectory(src)
