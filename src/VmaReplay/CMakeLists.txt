# This file will only be executed if VMA_BUILD_REPLAY is set to ON

if (WIN32)
    set(VMA_REPLAY_SOURCE_FILES
        Common.cpp
        Common.h
        Constants.cpp
        Constants.h
        VmaUsage.cpp
        VmaUsage.h
        VmaReplay.cpp
    )

    add_executable(VmaReplay ${VMA_REPLAY_SOURCE_FILES})
		add_dependencies(VmaReplay VulkanMemoryAllocator)

    # Enable multithreaded compiling
    target_compile_options(VmaReplay PRIVATE "/MP")

    target_link_libraries(
        VmaReplay

        PRIVATE
        Vulkan::Vulkan
    )
else()
    message(STATUS "VmaReplay is not supported on Linux")
endif()
