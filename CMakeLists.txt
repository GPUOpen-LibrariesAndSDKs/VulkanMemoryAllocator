cmake_minimum_required(VERSION 3.23...3.26)

project(VMA LANGUAGES CXX)

add_library(VulkanMemoryAllocator INTERFACE)
add_library(Vulkan::MemoryAllocator ALIAS VulkanMemoryAllocator)

target_sources(VulkanMemoryAllocator PUBLIC
    FILE_SET vk_mem_alloc
    BASE_DIRS include
    TYPE "HEADERS"
    FILES include/vk_mem_alloc.h
)

if (PROJECT_IS_TOP_LEVEL)
    include(GNUInstallDirs)

    set_target_properties(VulkanMemoryAllocator PROPERTIES EXPORT_NAME "MemoryAllocator")

    install(TARGETS VulkanMemoryAllocator EXPORT VulkanMemoryAllocator FILE_SET vk_mem_alloc)
    install(EXPORT VulkanMemoryAllocator NAMESPACE "Vulkan::" DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/VulkanMemoryAllocator")

    option(VMA_BUILD_DOCUMENTATION "Create and install the HTML based API documentation")
    if(VMA_BUILD_DOCUMENTATION)
        find_package(Doxygen REQUIRED)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        # note the option ALL which allows to build the docs together with the application
        add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()

    option(VMA_BUILD_SAMPLES "Build samples")
    if (VMA_BUILD_SAMPLES)
        add_subdirectory(src)
    endif()
endif()
