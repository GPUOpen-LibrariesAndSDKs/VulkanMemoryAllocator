# This file will only be executed if VMA_BUILD_REPLAY_APP is set to ON

if (WIN32)
    set(VMA_REPLAY_SOURCE_FILES
        Common.cpp
        Constants.cpp
        VmaReplay.cpp
        VmaUsage.cpp
    )

    add_executable(VmaReplayApp ${VMA_REPLAY_SOURCE_FILES})

    target_link_libraries(
        VmaReplayApp

        PRIVATE
        Vulkan::Vulkan
    )
else()
    message(STATUS "VmaReplay is not supported on Linux")
endif()
