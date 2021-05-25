IF (WIN32)
    set(VMA_REPLAY_SOURCE_FILES
        Common.cpp
        Constants.cpp
        VmaReplay.cpp
        VmaUsage.cpp
    )

    add_executable(VmaReplay ${VMA_REPLAY_SOURCE_FILES})

    target_link_libraries(
        VmaReplay

        PRIVATE
        Vulkan::Vulkan
    )
ELSE()
    message(STATUS "VmaReplay is not supported on Linux")
ENDIF()
