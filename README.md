# Vulkan Memory Allocator

This is a README file for Vulkan Memory Allocator library and accompanying sample application.

**Documentation:** See [Vulkan Memory Allocator](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/) (generated from Doxygen-style comments in [src/vk_mem_alloc.h](src/vk_mem_alloc.h)).

**License:** MIT. See [LICENSE.txt](LICENSE.txt).

**Changelog:** See [CHANGELOG.md](CHANGELOG.md).

**Build status:**

- Windows: [![Build status](https://ci.appveyor.com/api/projects/status/4vlcrb0emkaio2pn/branch/master?svg=true)](https://ci.appveyor.com/project/adam-sawicki-amd/vulkanmemoryallocator/branch/master)  
- Linux: [![Build Status](https://travis-ci.org/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.svg?branch=master)](https://travis-ci.org/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)

# Problem

Memory allocation and resource (buffer and image) creation in Vulkan is difficult (comparing to older graphics API-s, like D3D11 or OpenGL) for several reasons:

- It requires a lot of boilerplate code, just like everything else in Vulkan, because it is a low-level and high-performance API.
- There is additional level of indirection: `VkDeviceMemory` is allocated separately from creating `VkBuffer`/`VkImage` and they must be bound together. The binding cannot be changed later - resource must be recreated.
- Driver must be queried for supported memory heaps and memory types. Different IHVs provide different types of it.
- It is recommended practice to allocate bigger chunks of memory and assign parts of them to particular resources.

# Features

This library can help game developers to manage memory allocations and resource creation by offering some higher-level functions. Features of the library are divided into several layers, low level to high level:

1. Functions that help to choose correct and optimal memory type based on intended usage of the memory.
   - Required or preferred traits of the memory are expressed using higher-level description comparing to Vulkan flags.
2. Functions that allocate memory blocks, reserve and return parts of them (`VkDeviceMemory` + offset + size) to the user.
   - Library keeps track of allocated memory blocks, used and unused ranges inside them, finds best matching unused ranges for new allocations, takes all the rules of alignment into consideration.
3. Functions that can create an image/buffer, allocate memory for it and bind them together - all in one call.

Additional features:

- Thread-safety: Library is designed to be used by multithreaded code.
- Configuration: Fill optional members of CreateInfo structure to provide custom CPU memory allocator and other parameters.
- Customization: Predefine appropriate macros to provide your own implementation of all external facilities used by the library, from assert, mutex, and atomic, to vector and linked list. 
- Support for persistently mapped memory: Just allocate memory with appropriate flag and you get access to mapped pointer.
- Custom memory pools: Create a pool with desired parameters (e.g. fixed or limited maximum size) and allocate memory out of it.
- Defragmentation: Call one function and let the library move data around to free some memory blocks and make your allocations better compacted.
- Lost allocations: Allocate memory with appropriate flags and let the library remove allocations that are not used for many frames to make room for new ones.
- Statistics: Obtain detailed statistics about the amount of memory used, unused, number of allocated blocks, number of allocations etc. - globally, per memory heap, and per memory type.
- JSON dump: Obtain a string in JSON format with detailed map of internal state, including list of allocations and gaps between them.

# Prequisites

- Self-contained C++ library in single header file. No external dependencies other than standard C and C++ library and of course Vulkan.
- Public interface in C, in same convention as Vulkan API. Implementation in C++.
- Interface documented using Doxygen-style comments.
- Platform-independent, but developed and tested on Windows using Visual Studio.
- Error handling implemented by returning `VkResult` error codes - same way as in Vulkan.

# Read more

See **[Documentation](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/)**.

# See also

- **[Awesome Vulkan](https://github.com/vinjn/awesome-vulkan)** - a curated list of awesome Vulkan libraries, debuggers and resources.
- **[vulkan-malloc](https://github.com/dylanede/vulkan-malloc)** - Vulkan memory allocation library for Rust. Based on version 1 of this library. Author: Dylan Ede. License: MIT / Apache 2.0.
