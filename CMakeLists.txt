cmake_minimum_required(VERSION 3.9)

project(VulkanMemoryAllocator)

find_package(Vulkan REQUIRED)

# VulkanMemoryAllocator contains an sample application and VmaReplay which are not build by default
option(VMA_BUILD_SAMPLE "Build VulkanMemoryAllocator sample application" OFF)
option(VMA_BUILD_SAMPLE_SHADERS "Build VulkanMemoryAllocator sample application's shaders" OFF)
option(VMA_BUILD_REPLAY "Build VulkanMemoryAllocator replay application" OFF)

add_subdirectory(src)
