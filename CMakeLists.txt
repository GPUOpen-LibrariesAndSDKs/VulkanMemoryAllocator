cmake_minimum_required(VERSION 3.9)

project(VulkanMemoryAllocator)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

find_package(Vulkan REQUIRED)

# Put binaries into bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)
