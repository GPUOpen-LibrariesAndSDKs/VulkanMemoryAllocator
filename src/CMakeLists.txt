if(VMA_BUILD_EXAMPLE_APP_SHADERS)
    add_subdirectory(Shaders)
endif()

if(VMA_BUILD_REPLAY_APP)
    add_subdirectory(VmaReplay)
endif()

set(VMA_LIBRARY_SOURCE_FILES
    VmaUsage.cpp
)

# Use folder structure as filters in project map
foreach(FILE ${VMA_LIBRARY_SOURCE_FILES})
    get_filename_component(PARENT_DIR "${FILE}" PATH)

    string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")

    if("${FILE}" MATCHES ".*\\.cpp")
        set(GROUP "Source Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.hpp")
        set(GROUP "Header Files\\${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_library(VulkanMemoryAllocator ${VMA_LIBRARY_SOURCE_FILES})

set_target_properties(
    VulkanMemoryAllocator PROPERTIES

    CXX_EXTENSIONS OFF
    # Use C++14
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    VulkanMemoryAllocator

    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
    VulkanMemoryAllocator

    PUBLIC
    Vulkan::Vulkan
)

set(VMA_EXAMPLE_SOURCE_FILES
    Common.cpp
    SparseBindingTest.cpp
    Tests.cpp
    VulkanSample.cpp
)

# Use folder structure as filters in project map
foreach(FILE ${VMA_EXAMPLE_SOURCE_FILES})
    get_filename_component(PARENT_DIR "${FILE}" PATH)

    string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")

    if("${FILE}" MATCHES ".*\\.cpp")
        set(GROUP "Source Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.hpp")
        set(GROUP "Header Files\\${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

if (VMA_BUILD_EXAMPLE_APP)
    if(WIN32)
        add_executable(VmaExample ${VMA_EXAMPLE_SOURCE_FILES})

        # Visual Studio specific settings
        if(${CMAKE_GENERATOR} MATCHES "Visual Studio.*")
            # Use Unicode instead of multibyte set
            add_definitions(-DUNICODE -D_UNICODE)
            
            # Set VmaExample as startup project
            set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "VmaExample")
            
            # Enable multithreaded compiling
            target_compile_options(VmaExample PRIVATE "/MP")

            # Set working directory for Visual Studio debugger
            set_target_properties(
                VmaExample
                PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
            )
        endif()

        set_target_properties(
            VmaExample PROPERTIES

            CXX_EXTENSIONS OFF
            # Use C++14
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED ON
        )

        target_link_libraries(
            VmaExample

            PRIVATE
            VmaLibrary
        )
    else()
        message(STATUS "VmaExample application is not supported to Linux")
    endif()
endif()
