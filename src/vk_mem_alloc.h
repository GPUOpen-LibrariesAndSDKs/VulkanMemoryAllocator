//
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef AMD_VULKAN_MEMORY_ALLOCATOR_H
#define AMD_VULKAN_MEMORY_ALLOCATOR_H

/** \mainpage Vulkan Memory Allocator

Version 1.0.1 (2017-07-04)

Members grouped: see <a href="modules.html"><b>Modules</b></a>.

All members: see vk_mem_alloc.h.

\section problem Problem Statement

Memory allocation and resource (buffer and image) creation in Vulkan is
difficult (comparing to older graphics API-s, like D3D11 or OpenGL) for several
reasons:

- It requires a lot of boilerplate code, just like everything else in Vulkan,
  because it is a low-level and high-performance API.
- There is additional level of indirection: VkDeviceMemory is allocated
  separately from creating VkBuffer/VkImage and they must be bound together. The
  binding cannot be changed later - resource must be recreated.
- Driver must be queried for supported memory heaps and memory types. Different
  IHV-s provide different types of it.
- Resources that don't fit in VRAM are not automatically evicted to RAM.
  Developer must handle out-of-memory errors on his own.
- It is recommended practice to allocate bigger chunks of memory and assign
  parts of them to particular resources.

\section features Features

This library is helps game developers to manage memory allocations and resource
creation by offering some higher-level functions. Features of the library could
be divided into several layers, low level to high level:

-# Functions that help to choose correct and optimal memory type based on
   intended usage of the memory.
   - Required or preferred traits of the memory are expressed using higher-level
     description comparing to Vulkan flags.
-# Functions that allocate memory blocks, reserve and return parts of them
   (VkDeviceMemory + offset + size) to the user.
   - Library keeps track of allocated memory blocks, used and unused ranges
     inside them, finds best matching unused ranges for new allocations, takes
     all the rules of alignment into consideration.
-# Functions that can create an image/buffer, allocate memory for it and bind
   them together - all in one call.

\section prequisites Prequisites

- Self-contained C++ library in single header file. No external dependencies
  other than standard C and C++ library and of course Vulkan.
- Public interface in C, in same convention as Vulkan API. Implementation in
  C++.
- Interface documented using Doxygen-style comments.
- Platform-independent, but developed and tested on Windows using Visual Studio.
- Error handling implemented by returning VkResult error codes - same way as in
  Vulkan.

\section quick_start Quick Start

In your project code:

-# Include "vk_mem_alloc.h" file wherever you want to use the library.
-# In exacly one C++ file define following macro before include to build library
   implementation.


    #define VMA_IMPLEMENTATION
    #include "vk_mem_alloc.h"

At program startup:

-# Initialize Vulkan to have VkPhysicalDevice and VkDevice object.
-# Fill VmaAllocatorCreateInfo structure and create VmaAllocator object by
   calling vmaCreateAllocator().


    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = device;

    VmaAllocator allocator;
    vmaCreateAllocator(&allocatorInfo, &allocator);

When you want to create a buffer or image:

-# Fill VkBufferCreateInfo / VkImageCreateInfo structure.
-# Fill VmaMemoryRequirements structure.
-# Call vmaCreateBuffer() / vmaCreateImage() to get VkBuffer/VkImage with memory
   already allocated and bound to it.


    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = myBufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaMemoryRequirements memReq = {};
    memReq.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkBuffer buffer;
    vmaCreateBuffer(allocator, &bufferInfo, &memReq, &buffer, nullptr, nullptr);

When no longer needed, destroy your buffer or image using vmaDestroyBuffer() / vmaDestroyImage().
This function would also free memory bound to it.


    vmaDestroyBuffer(allocator, buffer);

\section configuration Configuration

Please check "CONFIGURATION SECTION" in the code to find macros that you can define
before each #include of this file or change directly in this file to provide
your own implementation of basic facilities like assert, min and max functions,
mutex etc. C++ STL is used by default, but changing these allows you to get rid
of any STL usage if you want, as many game developers tend to do.

\section custom_memory_allocator Custom memory allocator

You can use custom memory allocator by filling optional member
VmaAllocatorCreateInfo::pAllocationCallbacks. These functions will be passed to
Vulkan, as well as used by the library itself to make any CPU-side allocations.

\section thread_safety Thread safety

All calls to functions that take VmaAllocator as first parameter are safe to
call from multiple threads simultaneously, synchronized internally when needed.
*/

#include <vulkan/vulkan.h>

////////////////////////////////////////////////////////////////////////////////
/** \defgroup general General
@{
*/

VK_DEFINE_HANDLE(VmaAllocator)

/// Description of a Allocator to be created.
typedef struct VmaAllocatorCreateInfo
{
    /// Vulkan physical device.
    /** It must be valid throughout whole lifetime of created Allocator. */
    VkPhysicalDevice physicalDevice;
    /// Vulkan device.
    /** It must be valid throughout whole lifetime of created Allocator. */
    VkDevice device;
    /// Size of a single memory block to allocate for resources.
    /** Set to 0 to use default, which is currently 256 MB. */
    VkDeviceSize preferredLargeHeapBlockSize;
    /// Size of a single memory block to allocate for resources from a small heap <= 512 MB.
    /** Set to 0 to use default, which is currently 64 MB. */
    VkDeviceSize preferredSmallHeapBlockSize;
    /// Custom allocation callbacks.
    /** Optional, can be null. When specified, will also be used for all CPU-side memory allocations. */
    const VkAllocationCallbacks* pAllocationCallbacks;
} VmaAllocatorCreateInfo;

/// Creates Allocator object.
VkResult vmaCreateAllocator(
    const VmaAllocatorCreateInfo* pCreateInfo,
    VmaAllocator* pAllocator);

/// Destroys allocator object.
void vmaDestroyAllocator(
    VmaAllocator allocator);

/**
PhysicalDeviceProperties are fetched from physicalDevice by the allocator.
You can access it here, without fetching it again on your own.
*/
void vmaGetPhysicalDeviceProperties(
    VmaAllocator allocator,
    const VkPhysicalDeviceProperties** ppPhysicalDeviceProperties);

/**
PhysicalDeviceMemoryProperties are fetched from physicalDevice by the allocator.
You can access it here, without fetching it again on your own.
*/
void vmaGetMemoryProperties(
    VmaAllocator allocator,
    const VkPhysicalDeviceMemoryProperties** ppPhysicalDeviceMemoryProperties);

/**
\brief Given Memory Type Index, returns Property Flags of this memory type.

This is just a convenience function. Same information can be obtained using
vmaGetMemoryProperties().
*/
void vmaGetMemoryTypeProperties(
    VmaAllocator allocator,
    uint32_t memoryTypeIndex,
    VkMemoryPropertyFlags* pFlags);

typedef struct VmaStatInfo
{
    uint32_t AllocationCount;
    uint32_t SuballocationCount;
    uint32_t UnusedRangeCount;
    VkDeviceSize UsedBytes;
    VkDeviceSize UnusedBytes;
    VkDeviceSize SuballocationSizeMin, SuballocationSizeAvg, SuballocationSizeMax;
    VkDeviceSize UnusedRangeSizeMin, UnusedRangeSizeAvg, UnusedRangeSizeMax;
} VmaStatInfo;

/// General statistics from current state of Allocator.
struct VmaStats
{
    VmaStatInfo memoryType[VK_MAX_MEMORY_TYPES];
    VmaStatInfo memoryHeap[VK_MAX_MEMORY_HEAPS];
    VmaStatInfo total;
};

/// Retrieves statistics from current state of the Allocator.
void vmaCalculateStats(
    VmaAllocator allocator,
    VmaStats* pStats);

#ifndef VMA_STATS_STRING_ENABLED
   #define VMA_STATS_STRING_ENABLED 1
#endif

#if VMA_STATS_STRING_ENABLED

/// Builds and returns statistics as string in JSON format.
/** @param[out] ppStatsString Must be freed using vmaFreeStatsString() function.
*/
void vmaBuildStatsString(
    VmaAllocator allocator,
    char** ppStatsString,
    VkBool32 detailedMap);

void vmaFreeStatsString(
    VmaAllocator allocator,
    char* pStatsString);

#endif // #if VMA_STATS_STRING_ENABLED

/** @} */

////////////////////////////////////////////////////////////////////////////////
/** \defgroup layer1 Layer 1 Choosing Memory Type
@{
*/

typedef enum VmaMemoryUsage
{
    /// No intended memory usage specified.
    VMA_MEMORY_USAGE_UNKNOWN = 0,
    /// Memory will be used on device only, no need to be mapped on host.
    VMA_MEMORY_USAGE_GPU_ONLY = 1,
    /// Memory will be mapped on host. Could be used for transfer to device.
    VMA_MEMORY_USAGE_CPU_ONLY = 2,
    /// Memory will be used for frequent (dynamic) updates from host and reads on device.
    VMA_MEMORY_USAGE_CPU_TO_GPU = 3,
    /// Memory will be used for writing on device and readback on host.
    VMA_MEMORY_USAGE_GPU_TO_CPU = 4,
    VMA_MEMORY_USAGE_MAX_ENUM = 0x7FFFFFFF
} VmaMemoryUsage;

typedef struct VmaMemoryRequirements
{
    /** \brief Set to true if this allocation should have its own memory block.
    
    Use it for special, big resources, like fullscreen images used as attachments.
   
    This flag must also be used for host visible resources that you want to map
    simultaneously because otherwise they might end up as regions of the same
    VkDeviceMemory, while mapping same VkDeviceMemory multiple times is illegal.
    */
    VkBool32 ownMemory;
    /** \brief Intended usage of memory.
    
    Leave VMA_MEMORY_USAGE_UNKNOWN if you specify requiredFlags. You can also use both.
    */
    VmaMemoryUsage usage;
    /** \brief Flags that must be set in a Memory Type chosen for an allocation.
    
    Leave 0 if you specify requirement via usage. */
    VkMemoryPropertyFlags requiredFlags;
    /** \brief Flags that preferably should be set in a Memory Type chosen for an allocation.
    
    Set to 0 if no additional flags are prefered and only requiredFlags should be used.
    If not 0, it must be a superset or equal to requiredFlags. */
    VkMemoryPropertyFlags preferredFlags;
    /** \brief Set this flag to only try to allocate from existing VkDeviceMemory blocks and never create new such block.
    
    If new allocation cannot be placed in any of the existing blocks, allocation
    fails with VK_ERROR_OUT_OF_DEVICE_MEMORY error.
    
    It makes no sense to set ownMemory and neverAllocate at the same time. */
    VkBool32 neverAllocate;
} VmaMemoryRequirements;

/**
This algorithm tries to find a memory type that:

- Is allowed by memoryTypeBits.
- Contains all the flags from pMemoryRequirements->requiredFlags.
- Matches intended usage.
- Has as many flags from pMemoryRequirements->preferredFlags as possible.

\return Returns VK_ERROR_FEATURE_NOT_PRESENT if not found. Receiving such result
from this function or any other allocating function probably means that your
device doesn't support any memory type with requested features for the specific
type of resource you want to use it for. Please check parameters of your
resource, like image layout (OPTIMAL versus LINEAR) or mip level count.
*/
VkResult vmaFindMemoryTypeIndex(
    VmaAllocator allocator,
    uint32_t memoryTypeBits,
    const VmaMemoryRequirements* pMemoryRequirements,
    uint32_t* pMemoryTypeIndex);

/** @} */

////////////////////////////////////////////////////////////////////////////////
/** \defgroup layer2 Layer 2 Allocating Memory
@{
*/

/** \brief General purpose memory allocation.

@param[out] pMemory Allocated memory.
@param[out] pMemoryTypeIndex Optional. Index of memory type that has been chosen for this allocation.

You should free the memory using vmaFreeMemory().

All allocated memory is also automatically freed in vmaDestroyAllocator().

It is recommended to use vmaAllocateMemoryForBuffer(), vmaAllocateMemoryForImage(),
vmaCreateBuffer(), vmaCreateImage() instead whenever possible.
*/
VkResult vmaAllocateMemory(
    VmaAllocator allocator,
    const VkMemoryRequirements* pVkMemoryRequirements,
    const VmaMemoryRequirements* pVmaMemoryRequirements,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex);

/**
@param[out] pMemoryTypeIndex Optional. Pass null if you don't need this information.

You should free the memory using vmaFreeMemory().

All allocated memory is also automatically freed in vmaDestroyAllocator().
*/
VkResult vmaAllocateMemoryForBuffer(
    VmaAllocator allocator,
    VkBuffer buffer,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex);

/// Function similar to vmaAllocateMemoryForBuffer().
VkResult vmaAllocateMemoryForImage(
    VmaAllocator allocator,
    VkImage image,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex);

/// Frees memory previously allocated using vmaAllocateMemoryForBuffer() or vmaAllocateMemoryForImage().
void vmaFreeMemory(
    VmaAllocator allocator,
    const VkMappedMemoryRange* pMemory);

/**
Feel free to use vkMapMemory on these memory blocks on you own if you want, but
just for convenience and to make sure correct offset and size is always
specified, usage of vmaMapMemory() / vmaUnmapMemory() is recommended.
*/
VkResult vmaMapMemory(
    VmaAllocator allocator,
    const VkMappedMemoryRange* pMemory,
    void** ppData);

void vmaUnmapMemory(
    VmaAllocator allocator,
    const VkMappedMemoryRange* pMemory);

/** @} */

////////////////////////////////////////////////////////////////////////////////
/** \defgroup layer3 Layer 3 Creating Buffers and Images
@{
*/

/**
@param[out] pMemory Optional. Pass null if you don't need this information.
@param[out] pMemoryTypeIndex Optional. Pass null if you don't need this information.

This function automatically:

-# Creates buffer/image.
-# Allocates appropriate memory for it.
-# Binds the buffer/image with the memory.

You do not (and should not) pass returned pMemory to vmaFreeMemory. Only calling
vmaDestroyBuffer() / vmaDestroyImage() is required for objects created using
vmaCreateBuffer() / vmaCreateImage().

All allocated buffers and images are also automatically destroyed in
vmaDestroyAllocator(), along with their memory allocations.
*/
VkResult vmaCreateBuffer(
    VmaAllocator allocator,
    const VkBufferCreateInfo* pCreateInfo,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkBuffer* pBuffer,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex);

void vmaDestroyBuffer(
    VmaAllocator allocator,
    VkBuffer buffer);

/// Function similar to vmaCreateBuffer().
VkResult vmaCreateImage(
    VmaAllocator allocator,
    const VkImageCreateInfo* pCreateInfo,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkImage* pImage,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex);

void vmaDestroyImage(
    VmaAllocator allocator,
    VkImage image);

/** @} */

#ifdef VMA_IMPLEMENTATION

#include <cstdint>
#include <cstdlib>

/*******************************************************************************
CONFIGURATION SECTION

Define some of these macros before each #include of this header or change them
here if you need other then default behavior depending on your environment.
*/

// Define this macro to 1 to make the library use STL containers instead of its own implementation.
//#define VMA_USE_STL_CONTAINERS 1

/* Set this macro to 1 to make the library including and using STL containers:
std::pair, std::vector, std::list, std::unordered_map.

Set it to 0 or undefined to make the library using its own implementation of
the containers.
*/
#if VMA_USE_STL_CONTAINERS
   #define VMA_USE_STL_VECTOR 1
   #define VMA_USE_STL_UNORDERED_MAP 1
   #define VMA_USE_STL_LIST 1
#endif

#if VMA_USE_STL_VECTOR
   #include <vector>
#endif

#if VMA_USE_STL_UNORDERED_MAP
   #include <unordered_map>
#endif

#if VMA_USE_STL_LIST
   #include <list>
#endif

/*
Following headers are used in this CONFIGURATION section only, so feel free to
remove them if not needed.
*/
#include <cassert> // for assert
#include <algorithm> // for min, max
#include <mutex> // for std::mutex

#if !defined(_WIN32)
    #include <malloc.h> // for aligned_alloc()
#endif

// Normal assert to check for programmer's errors, especially in Debug configuration.
#ifndef VMA_ASSERT
   #ifdef _DEBUG
       #define VMA_ASSERT(expr)         assert(expr)
   #else
       #define VMA_ASSERT(expr)
   #endif
#endif

// Assert that will be called very often, like inside data structures e.g. operator[].
// Making it non-empty can make program slow.
#ifndef VMA_HEAVY_ASSERT
   #ifdef _DEBUG
       #define VMA_HEAVY_ASSERT(expr)   //VMA_ASSERT(expr)
   #else
       #define VMA_HEAVY_ASSERT(expr)
   #endif
#endif

#ifndef VMA_NULL
   // Value used as null pointer. Define it to e.g.: nullptr, NULL, 0, (void*)0.
   #define VMA_NULL   nullptr
#endif

#ifndef VMA_ALIGN_OF
   #define VMA_ALIGN_OF(type)       (__alignof(type))
#endif

#ifndef VMA_SYSTEM_ALIGNED_MALLOC
   #if defined(_WIN32)
       #define VMA_SYSTEM_ALIGNED_MALLOC(size, alignment)   (_aligned_malloc((size), (alignment)))
   #else
       #define VMA_SYSTEM_ALIGNED_MALLOC(size, alignment)   (aligned_alloc((alignment), (size) ))
   #endif
#endif

#ifndef VMA_SYSTEM_FREE
   #if defined(_WIN32)
       #define VMA_SYSTEM_FREE(ptr)   _aligned_free(ptr)
   #else
       #define VMA_SYSTEM_FREE(ptr)   free(ptr)
   #endif
#endif

#ifndef VMA_MIN
   #define VMA_MIN(v1, v2)    (std::min((v1), (v2)))
#endif

#ifndef VMA_MAX
   #define VMA_MAX(v1, v2)    (std::max((v1), (v2)))
#endif

#ifndef VMA_SWAP
   #define VMA_SWAP(v1, v2)   std::swap((v1), (v2))
#endif

#ifndef VMA_DEBUG_LOG
   #define VMA_DEBUG_LOG(format, ...)
   /*
   #define VMA_DEBUG_LOG(format, ...) do { \
       printf(format, __VA_ARGS__); \
       printf("\n"); \
   } while(false)
   */
#endif

// Define this macro to 1 to enable functions: vmaBuildStatsString, vmaFreeStatsString.
#if VMA_STATS_STRING_ENABLED
   static inline void VmaUint32ToStr(char* outStr, size_t strLen, uint32_t num)
   {
       _ultoa_s(num, outStr, strLen, 10);
   }
   static inline void VmaUint64ToStr(char* outStr, size_t strLen, uint64_t num)
   {
       _ui64toa_s(num, outStr, strLen, 10);
   }
#endif

#ifndef VMA_MUTEX
   class VmaMutex
   {
   public:
       VmaMutex() { }
       ~VmaMutex() { }
       void Lock() { m_Mutex.lock(); }
       void Unlock() { m_Mutex.unlock(); }
   private:
       std::mutex m_Mutex;
   };
   #define VMA_MUTEX VmaMutex
#endif

#ifndef VMA_BEST_FIT
   /**
   Main parameter for function assessing how good is a free suballocation for a new
   allocation request.

   - Set to 1 to use Best-Fit algorithm - prefer smaller blocks, as close to the
     size of requested allocations as possible.
   - Set to 0 to use Worst-Fit algorithm - prefer larger blocks, as large as
     possible.

   Experiments in special testing environment showed that Best-Fit algorithm is
   better.
   */
   #define VMA_BEST_FIT (1)
#endif

#ifndef VMA_DEBUG_ALWAYS_OWN_MEMORY
   /**
   Every object will have its own allocation.
   Define to 1 for debugging purposes only.
   */
   #define VMA_DEBUG_ALWAYS_OWN_MEMORY (0)
#endif

#ifndef VMA_DEBUG_ALIGNMENT
   /**
   Minimum alignment of all suballocations, in bytes.
   Set to more than 1 for debugging purposes only. Must be power of two.
   */
   #define VMA_DEBUG_ALIGNMENT (1)
#endif

#ifndef VMA_DEBUG_MARGIN
   /**
   Minimum margin between suballocations, in bytes.
   Set nonzero for debugging purposes only.
   */
   #define VMA_DEBUG_MARGIN (0)
#endif

#ifndef VMA_DEBUG_GLOBAL_MUTEX
   /**
   Set this to 1 for debugging purposes only, to enable single mutex protecting all
   entry calls to the library. Can be useful for debugging multithreading issues.
   */
   #define VMA_DEBUG_GLOBAL_MUTEX (0)
#endif

#ifndef VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY
   /**
   Minimum value for VkPhysicalDeviceLimits::bufferImageGranularity.
   Set to more than 1 for debugging purposes only. Must be power of two.
   */
   #define VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY (1)
#endif

#ifndef VMA_SMALL_HEAP_MAX_SIZE
   /// Maximum size of a memory heap in Vulkan to consider it "small".
   #define VMA_SMALL_HEAP_MAX_SIZE (512 * 1024 * 1024)
#endif

#ifndef VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE
   /// Default size of a block allocated as single VkDeviceMemory from a "large" heap.
   #define VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE (256 * 1024 * 1024)
#endif

#ifndef VMA_DEFAULT_SMALL_HEAP_BLOCK_SIZE
   /// Default size of a block allocated as single VkDeviceMemory from a "small" heap.
   #define VMA_DEFAULT_SMALL_HEAP_BLOCK_SIZE (64 * 1024 * 1024)
#endif

/*******************************************************************************
END OF CONFIGURATION
*/

static VkAllocationCallbacks VmaEmptyAllocationCallbacks = {
    VMA_NULL, VMA_NULL, VMA_NULL, VMA_NULL, VMA_NULL, VMA_NULL };

// Returns number of bits set to 1 in (v).
static inline uint32_t CountBitsSet(uint32_t v)
{
	uint32_t c = v - ((v >> 1) & 0x55555555);
	c = ((c >>  2) & 0x33333333) + (c & 0x33333333);
	c = ((c >>  4) + c) & 0x0F0F0F0F;
	c = ((c >>  8) + c) & 0x00FF00FF;
	c = ((c >> 16) + c) & 0x0000FFFF;
	return c;
}

// Aligns given value up to nearest multiply of align value. For example: VmaAlignUp(11, 8) = 16.
// Use types like uint32_t, uint64_t as T.
template <typename T>
static inline T VmaAlignUp(T val, T align)
{
	return (val + align - 1) / align * align;
}

// Division with mathematical rounding to nearest number.
template <typename T>
inline T VmaRoundDiv(T x, T y)
{
	return (x + (y / (T)2)) / y;
}
/*
Returns true if two memory blocks occupy overlapping pages.
ResourceA must be in less memory offset than ResourceB.

Algorithm is based on "Vulkan 1.0.39 - A Specification (with all registered Vulkan extensions)"
chapter 11.6 "Resource Memory Association", paragraph "Buffer-Image Granularity".
*/
static inline bool VmaBlocksOnSamePage(
    VkDeviceSize resourceAOffset,
    VkDeviceSize resourceASize,
    VkDeviceSize resourceBOffset,
    VkDeviceSize pageSize)
{
    VMA_ASSERT(resourceAOffset + resourceASize <= resourceBOffset && resourceASize > 0 && pageSize > 0);
    VkDeviceSize resourceAEnd = resourceAOffset + resourceASize - 1;
    VkDeviceSize resourceAEndPage = resourceAEnd & ~(pageSize - 1);
    VkDeviceSize resourceBStart = resourceBOffset;
    VkDeviceSize resourceBStartPage = resourceBStart & ~(pageSize - 1);
    return resourceAEndPage == resourceBStartPage;
}

enum VmaSuballocationType
{
    VMA_SUBALLOCATION_TYPE_FREE = 0,
    VMA_SUBALLOCATION_TYPE_UNKNOWN = 1,
    VMA_SUBALLOCATION_TYPE_BUFFER = 2,
    VMA_SUBALLOCATION_TYPE_IMAGE_UNKNOWN = 3,
    VMA_SUBALLOCATION_TYPE_IMAGE_LINEAR = 4,
    VMA_SUBALLOCATION_TYPE_IMAGE_OPTIMAL = 5,
    VMA_SUBALLOCATION_TYPE_MAX_ENUM = 0x7FFFFFFF
};

/*
Returns true if given suballocation types could conflict and must respect
VkPhysicalDeviceLimits::bufferImageGranularity. They conflict if one is buffer
or linear image and another one is optimal image. If type is unknown, behave
conservatively.
*/
static inline bool VmaIsBufferImageGranularityConflict(
    VmaSuballocationType suballocType1,
    VmaSuballocationType suballocType2)
{
    if(suballocType1 > suballocType2)
        VMA_SWAP(suballocType1, suballocType2);
    
    switch(suballocType1)
    {
    case VMA_SUBALLOCATION_TYPE_FREE:
        return false;
    case VMA_SUBALLOCATION_TYPE_UNKNOWN:
        return true;
    case VMA_SUBALLOCATION_TYPE_BUFFER:
        return
            suballocType2 == VMA_SUBALLOCATION_TYPE_IMAGE_UNKNOWN ||
            suballocType2 == VMA_SUBALLOCATION_TYPE_IMAGE_OPTIMAL;
    case VMA_SUBALLOCATION_TYPE_IMAGE_UNKNOWN:
        return
            suballocType2 == VMA_SUBALLOCATION_TYPE_IMAGE_UNKNOWN ||
            suballocType2 == VMA_SUBALLOCATION_TYPE_IMAGE_LINEAR ||
            suballocType2 == VMA_SUBALLOCATION_TYPE_IMAGE_OPTIMAL;
    case VMA_SUBALLOCATION_TYPE_IMAGE_LINEAR:
        return
            suballocType2 == VMA_SUBALLOCATION_TYPE_IMAGE_OPTIMAL;
    case VMA_SUBALLOCATION_TYPE_IMAGE_OPTIMAL:
        return false;
    default:
        VMA_ASSERT(0);
        return true;
    }
}

// Helper RAII class to lock a mutex in constructor and unlock it in destructor (at the end of scope).
struct VmaMutexLock
{
public:
    VmaMutexLock(VMA_MUTEX& mutex) : m_Mutex(mutex) { mutex.Lock(); }
    ~VmaMutexLock() { m_Mutex.Unlock(); }

private:
    VMA_MUTEX& m_Mutex;
};

#if VMA_DEBUG_GLOBAL_MUTEX
    static VMA_MUTEX gDebugGlobalMutex;
    #define VMA_DEBUG_GLOBAL_MUTEX_LOCK VmaMutexLock debugGlobalMutexLock(gDebugGlobalMutex);
#else
    #define VMA_DEBUG_GLOBAL_MUTEX_LOCK
#endif

// Minimum size of a free suballocation to register it in the free suballocation collection.
static const VkDeviceSize VMA_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER = 16;

/*
Performs binary search and returns iterator to first element that is greater or
equal to (key), according to comparison (cmp).

Cmp should return true if first argument is less than second argument.

Returned value is the found element, if present in the collection or place where
new element with value (key) should be inserted.
*/
template <typename IterT, typename KeyT, typename CmpT>
static IterT VmaBinaryFindFirstNotLess(IterT beg, IterT end, const KeyT &key, CmpT cmp)
{
	size_t down = 0, up = (end - beg);
	while(down < up)
	{
		const size_t mid = (down + up) / 2;
		if(cmp(*(beg+mid), key))
			down = mid + 1;
		else
			up = mid;
	}
	return beg + down;
}

////////////////////////////////////////////////////////////////////////////////
// Memory allocation

static void* VmaMalloc(const VkAllocationCallbacks* pAllocationCallbacks, size_t size, size_t alignment)
{
    if((pAllocationCallbacks != VMA_NULL) &&
        (pAllocationCallbacks->pfnAllocation != VMA_NULL))
    {
        return (*pAllocationCallbacks->pfnAllocation)(
            pAllocationCallbacks->pUserData,
            size,
            alignment,
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    }
    else
    {
        return VMA_SYSTEM_ALIGNED_MALLOC(size, alignment);
    }
}

static void VmaFree(const VkAllocationCallbacks* pAllocationCallbacks, void* ptr)
{
    if((pAllocationCallbacks != VMA_NULL) &&
        (pAllocationCallbacks->pfnFree != VMA_NULL))
    {
        (*pAllocationCallbacks->pfnFree)(pAllocationCallbacks->pUserData, ptr);
    }
    else
    {
        VMA_SYSTEM_FREE(ptr);
    }
}

template<typename T>
static T* VmaAllocate(const VkAllocationCallbacks* pAllocationCallbacks)
{
    return (T*)VmaMalloc(pAllocationCallbacks, sizeof(T), VMA_ALIGN_OF(T));
}

template<typename T>
static T* VmaAllocateArray(const VkAllocationCallbacks* pAllocationCallbacks, size_t count)
{
    return (T*)VmaMalloc(pAllocationCallbacks, sizeof(T) * count, VMA_ALIGN_OF(T));
}

#define vma_new(allocator, type)   new(VmaAllocate<type>(allocator))(type)

#define vma_new_array(allocator, type, count)   new(VmaAllocateArray<type>((allocator), (count)))(type)

template<typename T>
static void vma_delete(const VkAllocationCallbacks* pAllocationCallbacks, T* ptr)
{
    ptr->~T();
    VmaFree(pAllocationCallbacks, ptr);
}

template<typename T>
static void vma_delete_array(const VkAllocationCallbacks* pAllocationCallbacks, T* ptr, size_t count)
{
    if(ptr != VMA_NULL)
    {
        for(size_t i = count; i--; )
            ptr[i].~T();
        VmaFree(pAllocationCallbacks, ptr);
    }
}

// STL-compatible allocator.
template<typename T>
class VmaStlAllocator
{
public:
    const VkAllocationCallbacks* const m_pCallbacks;
    typedef T value_type;
    
    VmaStlAllocator(const VkAllocationCallbacks* pCallbacks) : m_pCallbacks(pCallbacks) { }
    template<typename U> VmaStlAllocator(const VmaStlAllocator<U>& src) : m_pCallbacks(src.m_pCallbacks) { }

    T* allocate(size_t n) { return VmaAllocateArray<T>(m_pCallbacks, n); }
    void deallocate(T* p, size_t n) { VmaFree(m_pCallbacks, p); }

    template<typename U>
    bool operator==(const VmaStlAllocator<U>& rhs) const
    {
        return m_pCallbacks == rhs.m_pCallbacks;
    }
    template<typename U>
    bool operator!=(const VmaStlAllocator<U>& rhs) const
    {
        return m_pCallbacks != rhs.m_pCallbacks;
    }

    VmaStlAllocator& operator=(const VmaStlAllocator& x) = delete;
};

#if VMA_USE_STL_VECTOR

#define VmaVector std::vector

template<typename T, typename allocatorT>
static void VectorInsert(std::vector<T, allocatorT>& vec, size_t index, const T& item)
{
    vec.insert(vec.begin() + index, item);
}

template<typename T, typename allocatorT>
static void VectorRemove(std::vector<T, allocatorT>& vec, size_t index)
{
    vec.erase(vec.begin() + index);
}

#else // #if VMA_USE_STL_VECTOR

/* Class with interface compatible with subset of std::vector.
T must be POD because constructors and destructors are not called and memcpy is
used for these objects. */
template<typename T, typename AllocatorT>
class VmaVector
{
public:
    VmaVector(const AllocatorT& allocator) :
        m_Allocator(allocator),
        m_pArray(VMA_NULL),
        m_Count(0),
        m_Capacity(0)
    {
    }

    VmaVector(AllocatorT& allocator) :
        m_Allocator(allocator),
        m_pArray(VMA_NULL),
        m_Count(0),
        m_Capacity(0)
    {
    }
    
    VmaVector(size_t count, AllocatorT& allocator) :
        m_Allocator(allocator),
        m_pArray(count ? (T*)VmaAllocateArray<T>(allocator->m_pCallbacks, count) : VMA_NULL),
        m_Count(count),
        m_Capacity(count)
    {
    }
    
    VmaVector(const VmaVector<T, AllocatorT>& src) :
        m_Allocator(src.m_Allocator),
        m_pArray(src.m_Count ? (T*)VmaAllocateArray<T>(src->m_pCallbacks, src.m_Count) : VMA_NULL),
        m_Count(src.m_Count),
        m_Capacity(src.m_Count)
    {
        if(m_Count != 0)
            memcpy(m_pArray, src.m_pArray, m_Count * sizeof(T));
    }
    
    ~VmaVector()
    {
        VmaFree(m_Allocator.m_pCallbacks, m_pArray);
    }

    VmaVector& operator=(const VmaVector<T, AllocatorT>& rhs)
    {
        if(&rhs != this)
        {
            Resize(rhs.m_Count);
            if(m_Count != 0)
                memcpy(m_pArray, rhs.m_pArray, m_Count * sizeof(T));
        }
        return *this;
    }
    
    bool empty() const { return m_Count == 0; }
    size_t size() const { return m_Count; }
    T* data() { return m_pArray; }
    const T* data() const { return m_pArray; }
    
    T& operator[](size_t index)
    {
        VMA_HEAVY_ASSERT(index < m_Count);
        return m_pArray[index];
    }
    const T& operator[](size_t index) const
    {
        VMA_HEAVY_ASSERT(index < m_Count);
        return m_pArray[index];
    }

    T& front()
    {
        VMA_HEAVY_ASSERT(m_Count > 0);
        return m_pArray[0];
    }
    const T& front() const
    {
        VMA_HEAVY_ASSERT(m_Count > 0);
        return m_pArray[0];
    }
    T& back()
    {
        VMA_HEAVY_ASSERT(m_Count > 0);
        return m_pArray[m_Count - 1];
    }
    const T& back() const
    {
        VMA_HEAVY_ASSERT(m_Count > 0);
        return m_pArray[m_Count - 1];
    }

    void reserve(size_t newCapacity, bool freeMemory = false)
    {
        newCapacity = VMA_MAX(newCapacity, m_Count);
        
        if((newCapacity < m_Capacity) && !freeMemory)
            newCapacity = m_Capacity;
        
        if(newCapacity != m_Capacity)
        {
            T* const newArray = newCapacity ? VmaAllocateArray<T>(m_Allocator, newCapacity) : VMA_NULL;
            if(m_Count != 0)
                memcpy(newArray, m_pArray, m_Count * sizeof(T));
            VmaFree(m_Allocator.m_pCallbacks, m_pArray);
            m_Capacity = newCapacity;
            m_pArray = newArray;
        }
    }

    void resize(size_t newCount, bool freeMemory = false)
    {
        size_t newCapacity = m_Capacity;
        if(newCount > m_Capacity)
            newCapacity = VMA_MAX(newCount, VMA_MAX(m_Capacity * 3 / 2, (size_t)8));
        else if(freeMemory)
            newCapacity = newCount;

        if(newCapacity != m_Capacity)
        {
            T* const newArray = newCapacity ? VmaAllocateArray<T>(m_Allocator.m_pCallbacks, newCapacity) : VMA_NULL;
            const size_t elementsToCopy = VMA_MIN(m_Count, newCount);
            if(elementsToCopy != 0)
                memcpy(newArray, m_pArray, elementsToCopy * sizeof(T));
            VmaFree(m_Allocator.m_pCallbacks, m_pArray);
            m_Capacity = newCapacity;
            m_pArray = newArray;
        }

        m_Count = newCount;
    }

    void clear(bool freeMemory = false)
    {
        resize(0, freeMemory);
    }

    void insert(size_t index, const T& src)
    {
        VMA_HEAVY_ASSERT(index <= m_Count);
        const size_t oldCount = size();
        resize(oldCount + 1);
        if(index < oldCount)
            memmove(m_pArray + (index + 1), m_pArray + index, (oldCount - index) * sizeof(T));
        m_pArray[index] = src;
    }

    void remove(size_t index)
    {
        VMA_HEAVY_ASSERT(index < m_Count);
        const size_t oldCount = size();
        if(index < oldCount - 1)
            memmove(m_pArray + index, m_pArray + (index + 1), (oldCount - index - 1) * sizeof(T));
        resize(oldCount - 1);
    }

    void push_back(const T& src)
    {
        const size_t newIndex = size();
        resize(newIndex + 1);
        m_pArray[newIndex] = src;
    }

    void pop_back()
    {
        VMA_HEAVY_ASSERT(m_Count > 0);
        resize(size() - 1);
    }

    void push_front(const T& src)
    {
        insert(0, src);
    }

    void pop_front()
    {
        VMA_HEAVY_ASSERT(m_Count > 0);
        remove(0);
    }

    typedef T* iterator;

    iterator begin() { return m_pArray; }
    iterator end() { return m_pArray + m_Count; }

private:
    AllocatorT m_Allocator;
    T* m_pArray;
    size_t m_Count;
    size_t m_Capacity;
};

template<typename T, typename allocatorT>
static void VectorInsert(VmaVector<T, allocatorT>& vec, size_t index, const T& item)
{
    vec.insert(index, item);
}

template<typename T, typename allocatorT>
static void VectorRemove(VmaVector<T, allocatorT>& vec, size_t index)
{
    vec.remove(index);
}

#endif // #if VMA_USE_STL_VECTOR

////////////////////////////////////////////////////////////////////////////////
// class VmaPoolAllocator

/*
Allocator for objects of type T using a list of arrays (pools) to speed up
allocation. Number of elements that can be allocated is not bounded because
allocator can create multiple blocks.
*/
template<typename T>
class VmaPoolAllocator
{
public:
    VmaPoolAllocator(const VkAllocationCallbacks* pAllocationCallbacks, size_t itemsPerBlock);
    ~VmaPoolAllocator();
    void Clear();
    T* Alloc();
    void Free(T* ptr);

private:
    union Item
    {
        uint32_t NextFreeIndex;
        T Value;
    };

    struct ItemBlock
    {
        Item* pItems;
        uint32_t FirstFreeIndex;
    };
    
    const VkAllocationCallbacks* m_pAllocationCallbacks;
    size_t m_ItemsPerBlock;
    VmaVector< ItemBlock, VmaStlAllocator<ItemBlock> > m_ItemBlocks;

    ItemBlock& CreateNewBlock();
};

template<typename T>
VmaPoolAllocator<T>::VmaPoolAllocator(const VkAllocationCallbacks* pAllocationCallbacks, size_t itemsPerBlock) :
    m_pAllocationCallbacks(pAllocationCallbacks),
    m_ItemsPerBlock(itemsPerBlock),
    m_ItemBlocks(VmaStlAllocator<ItemBlock>(pAllocationCallbacks))
{
    VMA_ASSERT(itemsPerBlock > 0);
}

template<typename T>
VmaPoolAllocator<T>::~VmaPoolAllocator()
{
    Clear();
}

template<typename T>
void VmaPoolAllocator<T>::Clear()
{
    for(size_t i = m_ItemBlocks.size(); i--; )
        vma_delete_array(m_pAllocationCallbacks, m_ItemBlocks[i].pItems, m_ItemsPerBlock);
    m_ItemBlocks.clear();
}

template<typename T>
T* VmaPoolAllocator<T>::Alloc()
{
    for(size_t i = m_ItemBlocks.size(); i--; )
    {
        ItemBlock& block = m_ItemBlocks[i];
        // This block has some free items: Use first one.
        if(block.FirstFreeIndex != UINT32_MAX)
        {
            Item* const pItem = &block.pItems[block.FirstFreeIndex];
            block.FirstFreeIndex = pItem->NextFreeIndex;
            return &pItem->Value;
        }
    }

    // No block has free item: Create new one and use it.
    ItemBlock& newBlock = CreateNewBlock();
    Item* const pItem = &newBlock.pItems[0];
    newBlock.FirstFreeIndex = pItem->NextFreeIndex;
    return &pItem->Value;
}

template<typename T>
void VmaPoolAllocator<T>::Free(T* ptr)
{
    // Search all memory blocks to find ptr.
    for(size_t i = 0; i < m_ItemBlocks.size(); ++i)
    {
        ItemBlock& block = m_ItemBlocks[i];
        
        // Casting to union.
        Item* pItemPtr;
        memcpy(&pItemPtr, &ptr, sizeof(pItemPtr));
        
        // Check if pItemPtr is in address range of this block.
        if((pItemPtr >= block.pItems) && (pItemPtr < block.pItems + m_ItemsPerBlock))
        {
            const uint32_t index = static_cast<uint32_t>(pItemPtr - block.pItems);
            pItemPtr->NextFreeIndex = block.FirstFreeIndex;
            block.FirstFreeIndex = index;
            return;
        }
    }
    VMA_ASSERT(0 && "Pointer doesn't belong to this memory pool.");
}

template<typename T>
typename VmaPoolAllocator<T>::ItemBlock& VmaPoolAllocator<T>::CreateNewBlock()
{
    ItemBlock newBlock = {
        vma_new_array(m_pAllocationCallbacks, Item, m_ItemsPerBlock), 0 };

    m_ItemBlocks.push_back(newBlock);

    // Setup singly-linked list of all free items in this block.
    for(uint32_t i = 0; i < m_ItemsPerBlock - 1; ++i)
        newBlock.pItems[i].NextFreeIndex = i + 1;
    newBlock.pItems[m_ItemsPerBlock - 1].NextFreeIndex = UINT32_MAX;
    return m_ItemBlocks.back();
}

////////////////////////////////////////////////////////////////////////////////
// class VmaRawList, VmaList

#if VMA_USE_STL_LIST

#define VmaList std::list

#else // #if VMA_USE_STL_LIST

template<typename T>
struct VmaListItem
{
    VmaListItem* pPrev;
    VmaListItem* pNext;
    T Value;
};

// Doubly linked list.
template<typename T>
class VmaRawList
{
public:
    typedef VmaListItem<T> ItemType;

    VmaRawList(const VkAllocationCallbacks* pAllocationCallbacks);
    ~VmaRawList();
    void Clear();

    size_t GetCount() const { return m_Count; }
    bool IsEmpty() const { return m_Count == 0; }

    ItemType* Front() { return m_pFront; }
    const ItemType* Front() const { return m_pFront; }
    ItemType* Back() { return m_pBack; }
    const ItemType* Back() const { return m_pBack; }

    ItemType* PushBack();
    ItemType* PushFront();
    ItemType* PushBack(const T& value);
    ItemType* PushFront(const T& value);
    void PopBack();
    void PopFront();
    
    // Item can be null - it means PushBack.
    ItemType* InsertBefore(ItemType* pItem);
    // Item can be null - it means PushFront.
    ItemType* InsertAfter(ItemType* pItem);

    ItemType* InsertBefore(ItemType* pItem, const T& value);
    ItemType* InsertAfter(ItemType* pItem, const T& value);

    void Remove(ItemType* pItem);

private:
    const VkAllocationCallbacks* const m_pAllocationCallbacks;
    VmaPoolAllocator<ItemType> m_ItemAllocator;
    ItemType* m_pFront;
    ItemType* m_pBack;
    size_t m_Count;

    // Declared not defined, to block copy constructor and assignment operator.
    VmaRawList(const VmaRawList<T>& src);
    VmaRawList<T>& operator=(const VmaRawList<T>& rhs);
};

template<typename T>
VmaRawList<T>::VmaRawList(const VkAllocationCallbacks* pAllocationCallbacks) :
    m_pAllocationCallbacks(pAllocationCallbacks),
    m_ItemAllocator(pAllocationCallbacks, 128),
    m_pFront(VMA_NULL),
    m_pBack(VMA_NULL),
    m_Count(0)
{
}

template<typename T>
VmaRawList<T>::~VmaRawList()
{
    // Intentionally not calling Clear, because that would be unnecessary
    // computations to return all items to m_ItemAllocator as free.
}

template<typename T>
void VmaRawList<T>::Clear()
{
    if(IsEmpty() == false)
    {
        ItemType* pItem = m_pBack;
        while(pItem != VMA_NULL)
        {
            ItemType* const pPrevItem = pItem->pPrev;
            m_ItemAllocator.Free(pItem);
            pItem = pPrevItem;
        }
        m_pFront = VMA_NULL;
        m_pBack = VMA_NULL;
        m_Count = 0;
    }
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::PushBack()
{
    ItemType* const pNewItem = m_ItemAllocator.Alloc();
    pNewItem->pNext = VMA_NULL;
    if(IsEmpty())
    {
        pNewItem->pPrev = VMA_NULL;
        m_pFront = pNewItem;
        m_pBack = pNewItem;
        m_Count = 1;
    }
    else
    {
        pNewItem->pPrev = m_pBack;
        m_pBack->pNext = pNewItem;
        m_pBack = pNewItem;
        ++m_Count;
    }
    return pNewItem;
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::PushFront()
{
    ItemType* const pNewItem = m_ItemAllocator.Alloc();
    pNewItem->pPrev = VMA_NULL;
    if(IsEmpty())
    {
        pNewItem->pNext = VMA_NULL;
        m_pFront = pNewItem;
        m_pBack = pNewItem;
        m_Count = 1;
    }
    else
    {
        pNewItem->pNext = m_pFront;
        m_pFront->pPrev = pNewItem;
        m_pFront = pNewItem;
        ++m_Count;
    }
    return pNewItem;
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::PushBack(const T& value)
{
    ItemType* const pNewItem = PushBack();
    pNewItem->Value = value;
    return pNewItem;
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::PushFront(const T& value)
{
    ItemType* const pNewItem = PushFront();
    pNewItem->Value = value;
    return pNewItem;
}

template<typename T>
void VmaRawList<T>::PopBack()
{
    VMA_HEAVY_ASSERT(m_Count > 0);
    ItemType* const pBackItem = m_pBack;
    ItemType* const pPrevItem = pBackItem->pPrev;
    if(pPrevItem != VMA_NULL)
        pPrevItem->pNext = VMA_NULL;
    m_pBack = pPrevItem;
    m_ItemAllocator.Free(pBackItem);
    --m_Count;
}

template<typename T>
void VmaRawList<T>::PopFront()
{
    VMA_HEAVY_ASSERT(m_Count > 0);
    ItemType* const pFrontItem = m_pFront;
    ItemType* const pNextItem = pFrontItem->pNext;
    if(pNextItem != VMA_NULL)
        pNextItem->pPrev = VMA_NULL;
    m_pFront = pNextItem;
    m_ItemAllocator.Free(pFrontItem);
    --m_Count;
}

template<typename T>
void VmaRawList<T>::Remove(ItemType* pItem)
{
    VMA_HEAVY_ASSERT(pItem != VMA_NULL);
    VMA_HEAVY_ASSERT(m_Count > 0);

    if(pItem->pPrev != VMA_NULL)
        pItem->pPrev->pNext = pItem->pNext;
    else
    {
        VMA_HEAVY_ASSERT(m_pFront == pItem);
        m_pFront = pItem->pNext;
    }

    if(pItem->pNext != VMA_NULL)
        pItem->pNext->pPrev = pItem->pPrev;
    else
    {
        VMA_HEAVY_ASSERT(m_pBack == pItem);
        m_pBack = pItem->pPrev;
    }

    m_ItemAllocator.Free(pItem);
    --m_Count;
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::InsertBefore(ItemType* pItem)
{
    if(pItem != VMA_NULL)
    {
        ItemType* const prevItem = pItem->pPrev;
        ItemType* const newItem = m_ItemAllocator.Alloc();
        newItem->pPrev = prevItem;
        newItem->pNext = pItem;
        pItem->pPrev = newItem;
        if(prevItem != VMA_NULL)
            prevItem->pNext = newItem;
        else
        {
            VMA_HEAVY_ASSERT(m_pFront = pItem);
            m_pFront = newItem;
        }
        ++m_Count;
        return newItem;
    }
    else
        return PushBack();
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::InsertAfter(ItemType* pItem)
{
    if(pItem != VMA_NULL)
    {
        ItemType* const nextItem = pItem->pNext;
        ItemType* const newItem = m_ItemAllocator.Alloc();
        newItem->pNext = nextItem;
        newItem->pPrev = pItem;
        pItem->pNext = newItem;
        if(nextItem != VMA_NULL)
            nextItem->pPrev = newItem;
        else
        {
            VMA_HEAVY_ASSERT(m_pBack = pItem);
            m_pBack = newItem;
        }
        ++m_Count;
        return newItem;
    }
    else
        return PushFront();
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::InsertBefore(ItemType* pItem, const T& value)
{
    ItemType* const newItem = InsertBefore(pItem);
    newItem->Value = value;
    return newItem;
}

template<typename T>
VmaListItem<T>* VmaRawList<T>::InsertAfter(ItemType* pItem, const T& value)
{
    ItemType* const newItem = InsertAfter(pItem);
    newItem->Value = value;
    return newItem;
}

template<typename T, typename AllocatorT>
class VmaList
{
public:
    class iterator
    {
    public:
        iterator() :
            m_pList(VMA_NULL),
            m_pItem(VMA_NULL)
        {
        }

        T& operator*() const
        {
            VMA_HEAVY_ASSERT(m_pItem != VMA_NULL);
            return m_pItem->Value;
        }
        T* operator->() const
        {
            VMA_HEAVY_ASSERT(m_pItem != VMA_NULL);
            return &m_pItem->Value;
        }

        iterator& operator++()
        {
            VMA_HEAVY_ASSERT(m_pItem != VMA_NULL);
            m_pItem = m_pItem->pNext;
            return *this;
        }
        iterator& operator--()
        {
            if(m_pItem != VMA_NULL)
                m_pItem = m_pItem->pPrev;
            else
            {
                VMA_HEAVY_ASSERT(!m_pList.IsEmpty());
                m_pItem = m_pList->Back();
            }
            return *this;
        }

        iterator operator++(int)
        {
            iterator result = *this;
            ++*this;
            return result;
        }
        iterator operator--(int)
        {
            iterator result = *this;
            --*this;
            return result;
        }

        bool operator==(const iterator& rhs) const
        {
            VMA_HEAVY_ASSERT(m_pList == rhs.m_pList);
            return m_pItem == rhs.m_pItem;
        }
        bool operator!=(const iterator& rhs) const
        {
            VMA_HEAVY_ASSERT(m_pList == rhs.m_pList);
            return m_pItem != rhs.m_pItem;
        }
        
    private:
        VmaRawList<T>* m_pList;
        VmaListItem<T>* m_pItem;

        iterator(VmaRawList<T>* pList, VmaListItem<T>* pItem) :
            m_pList(pList),
            m_pItem(pItem)
        {
        }

        friend class VmaList<T, AllocatorT>;
        friend class VmaList<T, AllocatorT>:: const_iterator;
    };

    class const_iterator
    {
    public:
        const_iterator() :
            m_pList(VMA_NULL),
            m_pItem(VMA_NULL)
        {
        }

        const_iterator(const iterator& src) :
            m_pList(src.m_pList),
            m_pItem(src.m_pItem)
        {
        }
        
        const T& operator*() const
        {
            VMA_HEAVY_ASSERT(m_pItem != VMA_NULL);
            return m_pItem->Value;
        }
        const T* operator->() const
        {
            VMA_HEAVY_ASSERT(m_pItem != VMA_NULL);
            return &m_pItem->Value;
        }

        const_iterator& operator++()
        {
            VMA_HEAVY_ASSERT(m_pItem != VMA_NULL);
            m_pItem = m_pItem->pNext;
            return *this;
        }
        const_iterator& operator--()
        {
            if(m_pItem != VMA_NULL)
                m_pItem = m_pItem->pPrev;
            else
            {
                VMA_HEAVY_ASSERT(!m_pList->IsEmpty());
                m_pItem = m_pList->Back();
            }
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator result = *this;
            ++*this;
            return result;
        }
        const_iterator operator--(int)
        {
            const_iterator result = *this;
            --*this;
            return result;
        }

        bool operator==(const const_iterator& rhs) const
        {
            VMA_HEAVY_ASSERT(m_pList == rhs.m_pList);
            return m_pItem == rhs.m_pItem;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            VMA_HEAVY_ASSERT(m_pList == rhs.m_pList);
            return m_pItem != rhs.m_pItem;
        }
        
    private:
        const_iterator(const VmaRawList<T>* pList, const VmaListItem<T>* pItem) :
            m_pList(pList),
            m_pItem(pItem)
        {
        }

        const VmaRawList<T>* m_pList;
        const VmaListItem<T>* m_pItem;

        friend class VmaList<T, AllocatorT>;
    };

    VmaList(AllocatorT& allocator) : m_RawList(allocator.m_pCallbacks) { }
    VmaList(const AllocatorT& allocator) : m_RawList(allocator.m_pCallbacks) { }

    bool empty() const { return m_RawList.IsEmpty(); }
    size_t size() const { return m_RawList.GetCount(); }

    iterator begin() { return iterator(&m_RawList, m_RawList.Front()); }
    iterator end() { return iterator(&m_RawList, VMA_NULL); }

    const_iterator cbegin() const { return const_iterator(&m_RawList, m_RawList.Front()); }
    const_iterator cend() const { return const_iterator(&m_RawList, VMA_NULL); }

    void clear() { m_RawList.Clear(); }
    void push_back(const T& value) { m_RawList.PushBack(value); }
    void erase(iterator it) { m_RawList.Remove(it.m_pItem); }
    iterator insert(iterator it, const T& value) { return iterator(&m_RawList, m_RawList.InsertBefore(it.m_pItem, value)); }

private:
    VmaRawList<T> m_RawList;
};

#endif // #if VMA_USE_STL_LIST

////////////////////////////////////////////////////////////////////////////////
// class VmaMap

#if VMA_USE_STL_UNORDERED_MAP

#define VmaPair std::pair

#define VMA_MAP_TYPE(KeyT, ValueT) \
    std::unordered_map< KeyT, ValueT, std::hash<KeyT>, std::equal_to<KeyT>, VmaStlAllocator< std::pair<KeyT, ValueT> > >

#else // #if VMA_USE_STL_UNORDERED_MAP

template<typename T1, typename T2>
struct VmaPair
{
    T1 first;
    T2 second;

    VmaPair() : first(), second() { }
    VmaPair(const T1& firstSrc, const T2& secondSrc) : first(firstSrc), second(secondSrc) { }
};

/* Class compatible with subset of interface of std::unordered_map.
KeyT, ValueT must be POD because they will be stored in VmaVector.
*/
template<typename KeyT, typename ValueT>
class VmaMap
{
public:
    typedef VmaPair<KeyT, ValueT> PairType;
    typedef PairType* iterator;

    VmaMap(VmaStlAllocator<PairType>& allocator) : m_Vector(allocator) { }
    VmaMap(const VmaStlAllocator<PairType>& allocator) : m_Vector(allocator) { }

    iterator begin() { return m_Vector.begin(); }
    iterator end() { return m_Vector.end(); }

    void insert(const PairType& pair);
    iterator find(const KeyT& key);
    void erase(iterator it);
    
private:
    VmaVector< PairType, VmaStlAllocator<PairType> > m_Vector;
};

#define VMA_MAP_TYPE(KeyT, ValueT) VmaMap<KeyT, ValueT>

template<typename FirstT, typename SecondT>
struct VmaPairFirstLess
{
    bool operator()(const VmaPair<FirstT, SecondT>& lhs, const VmaPair<FirstT, SecondT>& rhs) const
    {
        return lhs.first < rhs.first;
    }
    bool operator()(const VmaPair<FirstT, SecondT>& lhs, const FirstT& rhsFirst) const
    {
        return lhs.first < rhsFirst;
    }
};

template<typename KeyT, typename ValueT>
void VmaMap<KeyT, ValueT>::insert(const PairType& pair)
{
    const size_t indexToInsert = VmaBinaryFindFirstNotLess(
        m_Vector.data(),
        m_Vector.data() + m_Vector.size(),
        pair,
        VmaPairFirstLess<KeyT, ValueT>()) - m_Vector.data();
    VectorInsert(m_Vector, indexToInsert, pair);
}

template<typename KeyT, typename ValueT>
VmaPair<KeyT, ValueT>* VmaMap<KeyT, ValueT>::find(const KeyT& key)
{
    PairType* it = VmaBinaryFindFirstNotLess(
        m_Vector.data(),
        m_Vector.data() + m_Vector.size(),
        key,
        VmaPairFirstLess<KeyT, ValueT>());
    if((it != m_Vector.end()) && (it->first == key))
        return it;
    else
        return m_Vector.end();
}

template<typename KeyT, typename ValueT>
void VmaMap<KeyT, ValueT>::erase(iterator it)
{
    VectorRemove(m_Vector, it - m_Vector.begin());
}

#endif // #if VMA_USE_STL_UNORDERED_MAP

/*
Represents a region of VmaAllocation that is either assigned and returned as
allocated memory block or free.
*/
struct VmaSuballocation
{
    VkDeviceSize offset;
    VkDeviceSize size;
    VmaSuballocationType type;
};

typedef VmaList< VmaSuballocation, VmaStlAllocator<VmaSuballocation> > VmaSuballocationList;

// Parameters of an allocation.
struct VmaAllocationRequest
{
    VmaSuballocationList::iterator freeSuballocationItem;
    VkDeviceSize offset;
};

/* Single block of memory - VkDeviceMemory with all the data about its regions
assigned or free. */
class VmaAllocation
{
public:
    VkDeviceMemory m_hMemory;
    VkDeviceSize m_Size;
    uint32_t m_FreeCount;
    VkDeviceSize m_SumFreeSize;
    VmaSuballocationList m_Suballocations;
    // Suballocations that are free and have size greater than certain threshold.
    // Sorted by size, ascending.
    VmaVector< VmaSuballocationList::iterator, VmaStlAllocator< VmaSuballocationList::iterator > > m_FreeSuballocationsBySize;

    VmaAllocation(VmaAllocator hAllocator);

    ~VmaAllocation()
    {
        VMA_ASSERT(m_hMemory == VK_NULL_HANDLE);
    }

    // Always call after construction.
    void Init(VkDeviceMemory newMemory, VkDeviceSize newSize);
    // Always call before destruction.
    void Destroy(VmaAllocator allocator);
    
    // Validates all data structures inside this object. If not valid, returns false.
    bool Validate() const;
    
    // Tries to find a place for suballocation with given parameters inside this allocation.
    // If succeeded, fills pAllocationRequest and returns true.
    // If failed, returns false.
    bool CreateAllocationRequest(
        VkDeviceSize bufferImageGranularity,
        VkDeviceSize allocSize,
        VkDeviceSize allocAlignment,
        VmaSuballocationType allocType,
        VmaAllocationRequest* pAllocationRequest);

    // Checks if requested suballocation with given parameters can be placed in given pFreeSuballocItem.
    // If yes, fills pOffset and returns true. If no, returns false.
    bool CheckAllocation(
        VkDeviceSize bufferImageGranularity,
        VkDeviceSize allocSize,
        VkDeviceSize allocAlignment,
        VmaSuballocationType allocType,
        VmaSuballocationList::const_iterator freeSuballocItem,
        VkDeviceSize* pOffset) const;
    
    // Returns true if this allocation is empty - contains only single free suballocation.
    bool IsEmpty() const;

    // Makes actual allocation based on request. Request must already be checked
    // and valid.
    void Alloc(
        const VmaAllocationRequest& request,
        VmaSuballocationType type,
        VkDeviceSize allocSize);

    // Frees suballocation assigned to given memory region.
    void Free(const VkMappedMemoryRange* pMemory);

#if VMA_STATS_STRING_ENABLED
    void PrintDetailedMap(class VmaStringBuilder& sb) const;
#endif

private:
    // Given free suballocation, it merges it with following one, which must also be free.
    void MergeFreeWithNext(VmaSuballocationList::iterator item);
    // Releases given suballocation, making it free. Merges it with adjacent free
    // suballocations if applicable.
    void FreeSuballocation(VmaSuballocationList::iterator suballocItem);
    // Given free suballocation, it inserts it into sorted list of
    // m_FreeSuballocationsBySize if it's suitable.
    void RegisterFreeSuballocation(VmaSuballocationList::iterator item);
    // Given free suballocation, it removes it from sorted list of
    // m_FreeSuballocationsBySize if it's suitable.
    void UnregisterFreeSuballocation(VmaSuballocationList::iterator item);
};

// Allocation for an object that has its own private VkDeviceMemory.
struct VmaOwnAllocation
{
    VkDeviceMemory m_hMemory;
    VkDeviceSize m_Size;
    VmaSuballocationType m_Type;
};

struct VmaOwnAllocationMemoryHandleLess
{
    bool operator()(const VmaOwnAllocation& lhs, const VmaOwnAllocation& rhs) const
    {
        return lhs.m_hMemory < rhs.m_hMemory;
    }
    bool operator()(const VmaOwnAllocation& lhs, VkDeviceMemory rhsMem) const
    {
        return lhs.m_hMemory < rhsMem;
    }
};

/* Sequence of VmaAllocation. Represents memory blocks allocated for a specific
Vulkan memory type. */
struct VmaAllocationVector
{
    // Incrementally sorted by sumFreeSize, ascending.
    VmaVector< VmaAllocation*, VmaStlAllocator<VmaAllocation*> > m_Allocations;

    VmaAllocationVector(VmaAllocator hAllocator);
    ~VmaAllocationVector();

    bool IsEmpty() const { return m_Allocations.empty(); }

    // Tries to free memory from any if its Allocations.
    // Returns index of Allocation that the memory was freed from, or -1 if not found.
    size_t Free(const VkMappedMemoryRange* pMemory);

    // Performs single step in sorting m_Allocations. They may not be fully sorted
    // after this call.
    void IncrementallySortAllocations();
    
    // Adds statistics of this AllocationVector to pStats.
    void AddStats(VmaStats* pStats, uint32_t memTypeIndex, uint32_t memHeapIndex) const;

#if VMA_STATS_STRING_ENABLED
    void PrintDetailedMap(class VmaStringBuilder& sb) const;
#endif

private:
    VmaAllocator m_hAllocator;
};

// Main allocator object.
struct VmaAllocator_T
{
    VkDevice m_hDevice;
    bool m_AllocationCallbacksSpecified;
    VkAllocationCallbacks m_AllocationCallbacks;
    VkDeviceSize m_PreferredLargeHeapBlockSize;
    VkDeviceSize m_PreferredSmallHeapBlockSize;

    VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties m_MemProps;

    VmaAllocationVector* m_pAllocations[VK_MAX_MEMORY_TYPES];
    /* There can be at most one allocation that is completely empty - a
    hysteresis to avoid pessimistic case of alternating creation and destruction
    of a VkDeviceMemory. */
    bool m_HasEmptyAllocation[VK_MAX_MEMORY_TYPES];
    VMA_MUTEX m_AllocationsMutex[VK_MAX_MEMORY_TYPES];

    // Each vector is sorted by memory (handle value).
    typedef VmaVector< VmaOwnAllocation, VmaStlAllocator<VmaOwnAllocation> > OwnAllocationVectorType;
    OwnAllocationVectorType* m_pOwnAllocations[VK_MAX_MEMORY_TYPES];
    VMA_MUTEX m_OwnAllocationsMutex[VK_MAX_MEMORY_TYPES];

    // Sorted by first (VkBuffer handle value).
    VMA_MAP_TYPE(VkBuffer, VkMappedMemoryRange) m_BufferToMemoryMap;
    VMA_MUTEX m_BufferToMemoryMapMutex;
    // Sorted by first (VkImage handle value).
    VMA_MAP_TYPE(VkImage, VkMappedMemoryRange) m_ImageToMemoryMap;
    VMA_MUTEX m_ImageToMemoryMapMutex;
    
    VmaAllocator_T(const VmaAllocatorCreateInfo* pCreateInfo);
    ~VmaAllocator_T();

    const VkAllocationCallbacks* GetAllocationCallbacks() const
    {
        return m_AllocationCallbacksSpecified ? &m_AllocationCallbacks : 0;
    }

    VkDeviceSize GetPreferredBlockSize(uint32_t memTypeIndex) const;

    VkDeviceSize GetBufferImageGranularity() const
    {
        return VMA_MAX(
            static_cast<VkDeviceSize>(VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY),
            m_PhysicalDeviceProperties.limits.bufferImageGranularity);
    }

    uint32_t GetMemoryHeapCount() const { return m_MemProps.memoryHeapCount; }
    uint32_t GetMemoryTypeCount() const { return m_MemProps.memoryTypeCount; }

    // Main allocation function.
    VkResult AllocateMemory(
        const VkMemoryRequirements& vkMemReq,
        const VmaMemoryRequirements& vmaMemReq,
        VmaSuballocationType suballocType,
        VkMappedMemoryRange* pMemory,
        uint32_t* pMemoryTypeIndex);

    // Main deallocation function.
    void FreeMemory(const VkMappedMemoryRange* pMemory);

    void CalculateStats(VmaStats* pStats);

#if VMA_STATS_STRING_ENABLED
    void PrintDetailedMap(class VmaStringBuilder& sb);
#endif

private:
    VkPhysicalDevice m_PhysicalDevice;

    VkResult AllocateMemoryOfType(
        const VkMemoryRequirements& vkMemReq,
        const VmaMemoryRequirements& vmaMemReq,
        uint32_t memTypeIndex,
        VmaSuballocationType suballocType,
        VkMappedMemoryRange* pMemory);
    
    // Allocates and registers new VkDeviceMemory specifically for single allocation.
    VkResult AllocateOwnMemory(
        VkDeviceSize size,
        VmaSuballocationType suballocType,
        uint32_t memTypeIndex,
        VkMappedMemoryRange* pMemory);

    // Tries to free pMemory as Own Memory. Returns true if found and freed.
    bool FreeOwnMemory(const VkMappedMemoryRange* pMemory);
};

////////////////////////////////////////////////////////////////////////////////
// Memory allocation #2 after VmaAllocator_T definition

static void* VmaMalloc(VmaAllocator hAllocator, size_t size, size_t alignment)
{
    return VmaMalloc(&hAllocator->m_AllocationCallbacks, size, alignment);
}

static void VmaFree(VmaAllocator hAllocator, void* ptr)
{
    VmaFree(&hAllocator->m_AllocationCallbacks, ptr);
}

template<typename T>
static T* VmaAllocate(VmaAllocator hAllocator)
{
    return (T*)VmaMalloc(hAllocator, sizeof(T), VMA_ALIGN_OF(T));
}

template<typename T>
static T* VmaAllocateArray(VmaAllocator hAllocator, size_t count)
{
    return (T*)VmaMalloc(hAllocator, sizeof(T) * count, VMA_ALIGN_OF(T));
}

template<typename T>
static void vma_delete(VmaAllocator hAllocator, T* ptr)
{
    if(ptr != VMA_NULL)
    {
        ptr->~T();
        VmaFree(hAllocator, ptr);
    }
}

template<typename T>
static void vma_delete_array(VmaAllocator hAllocator, T* ptr, size_t count)
{
    if(ptr != VMA_NULL)
    {
        for(size_t i = count; i--; )
            ptr[i].~T();
        VmaFree(hAllocator, ptr);
    }
}

////////////////////////////////////////////////////////////////////////////////
// VmaStringBuilder

#if VMA_STATS_STRING_ENABLED

class VmaStringBuilder
{
public:
    VmaStringBuilder(VmaAllocator alloc) : m_Data(VmaStlAllocator<char>(alloc->GetAllocationCallbacks())) { }
    size_t GetLength() const { return m_Data.size(); }
    const char* GetData() const { return m_Data.data(); }

    void Add(char ch) { m_Data.push_back(ch); }
    void Add(const char* pStr);
    void AddNewLine() { Add('\n'); }
    void AddNumber(uint32_t num);
    void AddNumber(uint64_t num);
    void AddBool(bool b) { Add(b ? "true" : "false"); }
    void AddNull() { Add("null"); }
    void AddString(const char* pStr);

private:
    VmaVector< char, VmaStlAllocator<char> > m_Data;
};

void VmaStringBuilder::Add(const char* pStr)
{
    const size_t strLen = strlen(pStr);
    if(strLen > 0)
    {
        const size_t oldCount = m_Data.size();
        m_Data.resize(oldCount + strLen);
        memcpy(m_Data.data() + oldCount, pStr, strLen);
    }
}

void VmaStringBuilder::AddNumber(uint32_t num)
{
    char buf[11];
    VmaUint32ToStr(buf, sizeof(buf), num);
    Add(buf);
}

void VmaStringBuilder::AddNumber(uint64_t num)
{
    char buf[21];
    VmaUint64ToStr(buf, sizeof(buf), num);
    Add(buf);
}

void VmaStringBuilder::AddString(const char* pStr)
{
    Add('"');
    const size_t strLen = strlen(pStr);
    for(size_t i = 0; i < strLen; ++i)
    {
        char ch = pStr[i];
        if(ch == '\'')
            Add("\\\\");
        else if(ch == '"')
            Add("\\\"");
        else if(ch >= 32)
            Add(ch);
        else switch(ch)
        {
        case '\n':
            Add("\\n");
            break;
        case '\r':
            Add("\\r");
            break;
        case '\t':
            Add("\\t");
            break;
        default:
            VMA_ASSERT(0 && "Character not currently supported.");
            break;
        }
    }
    Add('"');
}

////////////////////////////////////////////////////////////////////////////////

// Correspond to values of enum VmaSuballocationType.
static const char* VMA_SUBALLOCATION_TYPE_NAMES[] = {
    "FREE",
    "UNKNOWN",
    "BUFFER",
    "IMAGE_UNKNOWN",
    "IMAGE_LINEAR",
    "IMAGE_OPTIMAL",
};

static void VmaPrintStatInfo(VmaStringBuilder& sb, const VmaStatInfo& stat)
{
    sb.Add("{ \"Allocations\": ");
    sb.AddNumber(stat.AllocationCount);
    sb.Add(", \"Suballocations\": ");
    sb.AddNumber(stat.SuballocationCount);
    sb.Add(", \"UnusedRanges\": ");
    sb.AddNumber(stat.UnusedRangeCount);
    sb.Add(", \"UsedBytes\": ");
    sb.AddNumber(stat.UsedBytes);
    sb.Add(", \"UnusedBytes\": ");
    sb.AddNumber(stat.UnusedBytes);
    sb.Add(", \"SuballocationSize\": { \"Min\": ");
    sb.AddNumber(stat.SuballocationSizeMin);
    sb.Add(", \"Avg\": ");
    sb.AddNumber(stat.SuballocationSizeAvg);
    sb.Add(", \"Max\": ");
    sb.AddNumber(stat.SuballocationSizeMax);
    sb.Add(" }, \"UnusedRangeSize\": { \"Min\": ");
    sb.AddNumber(stat.UnusedRangeSizeMin);
    sb.Add(", \"Avg\": ");
    sb.AddNumber(stat.UnusedRangeSizeAvg);
    sb.Add(", \"Max\": ");
    sb.AddNumber(stat.UnusedRangeSizeMax);
    sb.Add(" } }");
}

#endif // #if VMA_STATS_STRING_ENABLED

struct VmaSuballocationItemSizeLess
{
    bool operator()(
        const VmaSuballocationList::iterator lhs,
        const VmaSuballocationList::iterator rhs) const
    {
        return lhs->size < rhs->size;
    }
    bool operator()(
        const VmaSuballocationList::iterator lhs,
        VkDeviceSize rhsSize) const
    {
        return lhs->size < rhsSize;
    }
};

VmaAllocation::VmaAllocation(VmaAllocator hAllocator) :
    m_hMemory(VK_NULL_HANDLE),
    m_Size(0),
    m_FreeCount(0),
    m_SumFreeSize(0),
    m_Suballocations(VmaStlAllocator<VmaSuballocation>(hAllocator->GetAllocationCallbacks())),
    m_FreeSuballocationsBySize(VmaStlAllocator<VmaSuballocationList::iterator>(hAllocator->GetAllocationCallbacks()))
{
}

void VmaAllocation::Init(VkDeviceMemory newMemory, VkDeviceSize newSize)
{
    VMA_ASSERT(m_hMemory == VK_NULL_HANDLE);

    m_hMemory = newMemory;
    m_Size = newSize;
    m_FreeCount = 1;
    m_SumFreeSize = newSize;

    m_Suballocations.clear();
    m_FreeSuballocationsBySize.clear();

    VmaSuballocation suballoc = {};
    suballoc.offset = 0;
    suballoc.size = newSize;
    suballoc.type = VMA_SUBALLOCATION_TYPE_FREE;

    m_Suballocations.push_back(suballoc);
    VmaSuballocationList::iterator suballocItem = m_Suballocations.end();
    --suballocItem;
    m_FreeSuballocationsBySize.push_back(suballocItem);
}

void VmaAllocation::Destroy(VmaAllocator allocator)
{
    VMA_ASSERT(m_hMemory != VK_NULL_HANDLE);
    vkFreeMemory(allocator->m_hDevice, m_hMemory, allocator->GetAllocationCallbacks());
    m_hMemory = VK_NULL_HANDLE;
}

bool VmaAllocation::Validate() const
{
    if((m_hMemory == VK_NULL_HANDLE) ||
        (m_Size == 0) ||
        m_Suballocations.empty())
    {
        return false;
    }
    
    // Expected offset of new suballocation as calculates from previous ones.
    VkDeviceSize calculatedOffset = 0;
    // Expected number of free suballocations as calculated from traversing their list.
    uint32_t calculatedFreeCount = 0;
    // Expected sum size of free suballocations as calculated from traversing their list.
    VkDeviceSize calculatedSumFreeSize = 0;
    // Expected number of free suballocations that should be registered in
    // m_FreeSuballocationsBySize calculated from traversing their list.
    size_t freeSuballocationsToRegister = 0;
    // True if previous visisted suballocation was free.
    bool prevFree = false;

    for(VmaSuballocationList::const_iterator suballocItem = m_Suballocations.cbegin();
        suballocItem != m_Suballocations.cend();
        ++suballocItem)
    {
        const VmaSuballocation& subAlloc = *suballocItem;
        
        // Actual offset of this suballocation doesn't match expected one.
        if(subAlloc.offset != calculatedOffset)
            return false;

        const bool currFree = (subAlloc.type == VMA_SUBALLOCATION_TYPE_FREE);
        // Two adjacent free suballocations are invalid. They should be merged.
        if(prevFree && currFree)
            return false;
        prevFree = currFree;

        if(currFree)
        {
            calculatedSumFreeSize += subAlloc.size;
            ++calculatedFreeCount;
            if(subAlloc.size >= VMA_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER)
                ++freeSuballocationsToRegister;
        }

        calculatedOffset += subAlloc.size;
    }

    // Number of free suballocations registered in m_FreeSuballocationsBySize doesn't
    // match expected one.
    if(m_FreeSuballocationsBySize.size() != freeSuballocationsToRegister)
        return false;

    VkDeviceSize lastSize = 0;
    for(size_t i = 0; i < m_FreeSuballocationsBySize.size(); ++i)
    {
        VmaSuballocationList::iterator suballocItem = m_FreeSuballocationsBySize[i];
        
        // Only free suballocations can be registered in m_FreeSuballocationsBySize.
        if(suballocItem->type != VMA_SUBALLOCATION_TYPE_FREE)
            return false;
        // They must be sorted by size ascending.
        if(suballocItem->size < lastSize)
            return false;

        lastSize = suballocItem->size;
    }

    // Check if totals match calculacted values.
    return
        (calculatedOffset == m_Size) &&
        (calculatedSumFreeSize == m_SumFreeSize) &&
        (calculatedFreeCount == m_FreeCount);
}

/*
How many suitable free suballocations to analyze before choosing best one.
- Set to 1 to use First-Fit algorithm - first suitable free suballocation will
  be chosen.
- Set to UINT32_MAX to use Best-Fit/Worst-Fit algorithm - all suitable free
  suballocations will be analized and best one will be chosen.
- Any other value is also acceptable.
*/
//static const uint32_t MAX_SUITABLE_SUBALLOCATIONS_TO_CHECK = 8;

bool VmaAllocation::CreateAllocationRequest(
    VkDeviceSize bufferImageGranularity,
    VkDeviceSize allocSize,
    VkDeviceSize allocAlignment,
    VmaSuballocationType allocType,
    VmaAllocationRequest* pAllocationRequest)
{
    VMA_ASSERT(allocSize > 0);
    VMA_ASSERT(allocType != VMA_SUBALLOCATION_TYPE_FREE);
    VMA_ASSERT(pAllocationRequest != VMA_NULL);
    VMA_HEAVY_ASSERT(Validate());

    // There is not enough total free space in this allocation to fullfill the request: Early return.
    if(m_SumFreeSize < allocSize)
        return false;

    // Old brute-force algorithm, linearly searching suballocations.
    /*
    uint32_t suitableSuballocationsFound = 0;
    for(VmaSuballocationList::iterator suballocItem = suballocations.Front();
        suballocItem != VMA_NULL &&
            suitableSuballocationsFound < MAX_SUITABLE_SUBALLOCATIONS_TO_CHECK;
        suballocItem = suballocItem->Next)
    {
        if(suballocItem->Value.type == VMA_SUBALLOCATION_TYPE_FREE)
        {
            VkDeviceSize offset = 0, cost = 0;
            if(CheckAllocation(bufferImageGranularity, allocSize, allocAlignment, allocType, suballocItem, &offset, &cost))
            {
                ++suitableSuballocationsFound;
                if(cost < costLimit)
                {
                    pAllocationRequest->freeSuballocationItem = suballocItem;
                    pAllocationRequest->offset = offset;
                    pAllocationRequest->cost = cost;
                    if(cost == 0)
                        return true;
                    costLimit = cost;
                    betterSuballocationFound = true;
                }
            }
        }
    }
    */

    // New algorithm, efficiently searching freeSuballocationsBySize.
    const size_t freeSuballocCount = m_FreeSuballocationsBySize.size();
    if(freeSuballocCount > 0)
    {
        if(VMA_BEST_FIT)
        {
            // Find first free suballocation with size not less than allocSize.
            VmaSuballocationList::iterator* const it = VmaBinaryFindFirstNotLess(
                m_FreeSuballocationsBySize.data(),
                m_FreeSuballocationsBySize.data() + freeSuballocCount,
                allocSize,
                VmaSuballocationItemSizeLess());
            size_t index = it - m_FreeSuballocationsBySize.data();
            for(; index < freeSuballocCount; ++index)
            {
                VkDeviceSize offset = 0;
                const VmaSuballocationList::iterator suballocItem = m_FreeSuballocationsBySize[index];
                if(CheckAllocation(bufferImageGranularity, allocSize, allocAlignment, allocType, suballocItem, &offset))
                {
                    pAllocationRequest->freeSuballocationItem = suballocItem;
                    pAllocationRequest->offset = offset;
                    return true;
                }
            }
        }
        else
        {
            // Search staring from biggest suballocations.
            for(size_t index = freeSuballocCount; index--; )
            {
                VkDeviceSize offset = 0;
                const VmaSuballocationList::iterator suballocItem = m_FreeSuballocationsBySize[index];
                if(CheckAllocation(bufferImageGranularity, allocSize, allocAlignment, allocType, suballocItem, &offset))
                {
                    pAllocationRequest->freeSuballocationItem = suballocItem;
                    pAllocationRequest->offset = offset;
                    return true;
                }
            }
        }
    }

    return false;
}

bool VmaAllocation::CheckAllocation(
    VkDeviceSize bufferImageGranularity,
    VkDeviceSize allocSize,
    VkDeviceSize allocAlignment,
    VmaSuballocationType allocType,
    VmaSuballocationList::const_iterator freeSuballocItem,
    VkDeviceSize* pOffset) const
{
    VMA_ASSERT(allocSize > 0);
    VMA_ASSERT(allocType != VMA_SUBALLOCATION_TYPE_FREE);
    VMA_ASSERT(freeSuballocItem != m_Suballocations.cend());
    VMA_ASSERT(pOffset != VMA_NULL);

    const VmaSuballocation& suballoc = *freeSuballocItem;
    VMA_ASSERT(suballoc.type == VMA_SUBALLOCATION_TYPE_FREE);

    // Size of this suballocation is too small for this request: Early return.
    if(suballoc.size < allocSize)
        return false;

    // Start from offset equal to beginning of this suballocation.
    *pOffset = suballoc.offset;
    
    // Apply VMA_DEBUG_MARGIN at the beginning.
    if((VMA_DEBUG_MARGIN > 0) && freeSuballocItem != m_Suballocations.cbegin())
        *pOffset += VMA_DEBUG_MARGIN;
    
    // Apply alignment.
    const VkDeviceSize alignment = VMA_MAX(allocAlignment, static_cast<VkDeviceSize>(VMA_DEBUG_ALIGNMENT));
    *pOffset = VmaAlignUp(*pOffset, alignment);
    
    // Check previous suballocations for BufferImageGranularity conflicts.
    // Make bigger alignment if necessary.
    if(bufferImageGranularity > 1)
    {
        bool bufferImageGranularityConflict = false;
        VmaSuballocationList::const_iterator prevSuballocItem = freeSuballocItem;
        while(prevSuballocItem != m_Suballocations.cbegin())
        {
            --prevSuballocItem;
            const VmaSuballocation& prevSuballoc = *prevSuballocItem;
            if(VmaBlocksOnSamePage(prevSuballoc.offset, prevSuballoc.size, *pOffset, bufferImageGranularity))
            {
                if(VmaIsBufferImageGranularityConflict(prevSuballoc.type, allocType))
                {
                    bufferImageGranularityConflict = true;
                    break;
                }
            }
            else
                // Already on previous page.
                break;
        }
        if(bufferImageGranularityConflict)
            *pOffset = VmaAlignUp(*pOffset, bufferImageGranularity);
    }
    
    // Calculate padding at the beginning based on current offset.
    const VkDeviceSize paddingBegin = *pOffset - suballoc.offset;

    // Calculate required margin at the end if this is not last suballocation.
    VmaSuballocationList::const_iterator next = freeSuballocItem;
    ++next;
    const VkDeviceSize requiredEndMargin =
        (next != m_Suballocations.cend()) ? VMA_DEBUG_MARGIN : 0;

    // Fail if requested size plus margin before and after is bigger than size of this suballocation.
    if(paddingBegin + allocSize + requiredEndMargin > suballoc.size)
        return false;

    // Check next suballocations for BufferImageGranularity conflicts.
    // If conflict exists, allocation cannot be made here.
    if(bufferImageGranularity > 1)
    {
        VmaSuballocationList::const_iterator nextSuballocItem = freeSuballocItem;
        ++nextSuballocItem;
        while(nextSuballocItem != m_Suballocations.cend())
        {
            const VmaSuballocation& nextSuballoc = *nextSuballocItem;
            if(VmaBlocksOnSamePage(*pOffset, allocSize, nextSuballoc.offset, bufferImageGranularity))
            {
                if(VmaIsBufferImageGranularityConflict(allocType, nextSuballoc.type))
                    return false;
            }
            else
                // Already on next page.
                break;
            ++nextSuballocItem;
        }
    }

    // All tests passed: Success. pOffset is already filled.
    return true;
}

bool VmaAllocation::IsEmpty() const
{
    return (m_Suballocations.size() == 1) && (m_FreeCount == 1);
}

void VmaAllocation::Alloc(
    const VmaAllocationRequest& request,
    VmaSuballocationType type,
    VkDeviceSize allocSize)
{
    VMA_ASSERT(request.freeSuballocationItem != m_Suballocations.end());
    VmaSuballocation& suballoc = *request.freeSuballocationItem;
    // Given suballocation is a free block.
    VMA_ASSERT(suballoc.type == VMA_SUBALLOCATION_TYPE_FREE);
    // Given offset is inside this suballocation.
    VMA_ASSERT(request.offset >= suballoc.offset);
    const VkDeviceSize paddingBegin = request.offset - suballoc.offset;
    VMA_ASSERT(suballoc.size >= paddingBegin + allocSize);
    const VkDeviceSize paddingEnd = suballoc.size - paddingBegin - allocSize;

    // Unregister this free suballocation from m_FreeSuballocationsBySize and update
    // it to become used.
    UnregisterFreeSuballocation(request.freeSuballocationItem);

    suballoc.offset = request.offset;
    suballoc.size = allocSize;
    suballoc.type = type;

    // If there are any free bytes remaining at the end, insert new free suballocation after current one.
    if(paddingEnd)
    {
        VmaSuballocation paddingSuballoc = {};
        paddingSuballoc.offset = request.offset + allocSize;
        paddingSuballoc.size = paddingEnd;
        paddingSuballoc.type = VMA_SUBALLOCATION_TYPE_FREE;
        VmaSuballocationList::iterator next = request.freeSuballocationItem;
        ++next;
        const VmaSuballocationList::iterator paddingEndItem =
            m_Suballocations.insert(next, paddingSuballoc);
        RegisterFreeSuballocation(paddingEndItem);
    }

    // If there are any free bytes remaining at the beginning, insert new free suballocation before current one.
    if(paddingBegin)
    {
        VmaSuballocation paddingSuballoc = {};
        paddingSuballoc.offset = request.offset - paddingBegin;
        paddingSuballoc.size = paddingBegin;
        paddingSuballoc.type = VMA_SUBALLOCATION_TYPE_FREE;
        const VmaSuballocationList::iterator paddingBeginItem =
            m_Suballocations.insert(request.freeSuballocationItem, paddingSuballoc);
        RegisterFreeSuballocation(paddingBeginItem);
    }

    // Update totals.
    m_FreeCount = m_FreeCount - 1;
    if(paddingBegin > 0)
        ++m_FreeCount;
    if(paddingEnd > 0)
        ++m_FreeCount;
    m_SumFreeSize -= allocSize;
}

void VmaAllocation::FreeSuballocation(VmaSuballocationList::iterator suballocItem)
{
    // Change this suballocation to be marked as free.
    VmaSuballocation& suballoc = *suballocItem;
    suballoc.type = VMA_SUBALLOCATION_TYPE_FREE;
    
    // Update totals.
    ++m_FreeCount;
    m_SumFreeSize += suballoc.size;

    // Merge with previous and/or next suballocation if it's also free.
    bool mergeWithNext = false;
    bool mergeWithPrev = false;
    
    VmaSuballocationList::iterator nextItem = suballocItem;
    ++nextItem;
    if((nextItem != m_Suballocations.end()) && (nextItem->type == VMA_SUBALLOCATION_TYPE_FREE))
        mergeWithNext = true;

    VmaSuballocationList::iterator prevItem = suballocItem;
    if(suballocItem != m_Suballocations.begin())
    {
        --prevItem;
        if(prevItem->type == VMA_SUBALLOCATION_TYPE_FREE)
            mergeWithPrev = true;
    }

    if(mergeWithNext)
    {
        UnregisterFreeSuballocation(nextItem);
        MergeFreeWithNext(suballocItem);
    }

    if(mergeWithPrev)
    {
        UnregisterFreeSuballocation(prevItem);
        MergeFreeWithNext(prevItem);
        RegisterFreeSuballocation(prevItem);
    }
    else
        RegisterFreeSuballocation(suballocItem);
}

void VmaAllocation::Free(const VkMappedMemoryRange* pMemory)
{
    // If suballocation to free has offset smaller than half of allocation size, search forward.
    // Otherwise search backward.
    const bool forwardDirection = pMemory->offset < (m_Size / 2);
    if(forwardDirection)
    {
        for(VmaSuballocationList::iterator suballocItem = m_Suballocations.begin();
            suballocItem != m_Suballocations.end();
            ++suballocItem)
        {
            VmaSuballocation& suballoc = *suballocItem;
            if(suballoc.offset == pMemory->offset)
            {
                FreeSuballocation(suballocItem);
                VMA_HEAVY_ASSERT(Validate());
                return;
            }
        }
        VMA_ASSERT(0 && "Not found!");
    }
    else
    {
        for(VmaSuballocationList::iterator suballocItem = m_Suballocations.begin();
            suballocItem != m_Suballocations.end();
            ++suballocItem)
        {
            VmaSuballocation& suballoc = *suballocItem;
            if(suballoc.offset == pMemory->offset)
            {
                FreeSuballocation(suballocItem);
                VMA_HEAVY_ASSERT(Validate());
                return;
            }
        }
        VMA_ASSERT(0 && "Not found!");
    }
}

#if VMA_STATS_STRING_ENABLED

void VmaAllocation::PrintDetailedMap(class VmaStringBuilder& sb) const
{
    sb.Add("{\n\t\t\t\"Bytes\": ");
    sb.AddNumber(m_Size);
    sb.Add(",\n\t\t\t\"FreeBytes\": ");
    sb.AddNumber(m_SumFreeSize);
    sb.Add(",\n\t\t\t\"Suballocations\": ");
    sb.AddNumber(m_Suballocations.size());
    sb.Add(",\n\t\t\t\"FreeSuballocations\": ");
    sb.AddNumber(m_FreeCount);
    sb.Add(",\n\t\t\t\"SuballocationList\": [");

    size_t i = 0;
    for(VmaSuballocationList::const_iterator suballocItem = m_Suballocations.cbegin();
        suballocItem != m_Suballocations.cend();
        ++suballocItem, ++i)
    {
        if(i > 0)
            sb.Add(",\n\t\t\t\t{ \"Type\": ");
        else
            sb.Add("\n\t\t\t\t{ \"Type\": ");
        sb.AddString(VMA_SUBALLOCATION_TYPE_NAMES[suballocItem->type]);
        sb.Add(", \"Size\": ");
        sb.AddNumber(suballocItem->size);
        sb.Add(", \"Offset\": ");
        sb.AddNumber(suballocItem->offset);
        sb.Add(" }");
    }

    sb.Add("\n\t\t\t]\n\t\t}");
}

#endif // #if VMA_STATS_STRING_ENABLED

void VmaAllocation::MergeFreeWithNext(VmaSuballocationList::iterator item)
{
    VMA_ASSERT(item != m_Suballocations.end());
    VMA_ASSERT(item->type == VMA_SUBALLOCATION_TYPE_FREE);
    
    VmaSuballocationList::iterator nextItem = item;
    ++nextItem;
    VMA_ASSERT(nextItem != m_Suballocations.end());
    VMA_ASSERT(nextItem->type == VMA_SUBALLOCATION_TYPE_FREE);

    item->size += nextItem->size;
    --m_FreeCount;
    m_Suballocations.erase(nextItem);
}

void VmaAllocation::RegisterFreeSuballocation(VmaSuballocationList::iterator item)
{
    VMA_ASSERT(item->type == VMA_SUBALLOCATION_TYPE_FREE);
    VMA_ASSERT(item->size > 0);

    if(item->size >= VMA_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER)
    {
        if(m_FreeSuballocationsBySize.empty())
            m_FreeSuballocationsBySize.push_back(item);
        else
        {
            VmaSuballocationList::iterator* const it = VmaBinaryFindFirstNotLess(
                m_FreeSuballocationsBySize.data(),
                m_FreeSuballocationsBySize.data() + m_FreeSuballocationsBySize.size(),
                item,
                VmaSuballocationItemSizeLess());
            size_t index = it - m_FreeSuballocationsBySize.data();
            VectorInsert(m_FreeSuballocationsBySize, index, item);
        }
    }
}

void VmaAllocation::UnregisterFreeSuballocation(VmaSuballocationList::iterator item)
{
    VMA_ASSERT(item->type == VMA_SUBALLOCATION_TYPE_FREE);
    VMA_ASSERT(item->size > 0);

    if(item->size >= VMA_MIN_FREE_SUBALLOCATION_SIZE_TO_REGISTER)
    {
        VmaSuballocationList::iterator* const it = VmaBinaryFindFirstNotLess(
            m_FreeSuballocationsBySize.data(),
            m_FreeSuballocationsBySize.data() + m_FreeSuballocationsBySize.size(),
            item,
            VmaSuballocationItemSizeLess());
        for(size_t index = it - m_FreeSuballocationsBySize.data();
            index < m_FreeSuballocationsBySize.size();
            ++index)
        {
            if(m_FreeSuballocationsBySize[index] == item)
            {
                VectorRemove(m_FreeSuballocationsBySize, index);
                return;
            }
            VMA_ASSERT((m_FreeSuballocationsBySize[index]->size == item->size) && "Not found.");
        }
        VMA_ASSERT(0 && "Not found.");
    }
}

static void InitStatInfo(VmaStatInfo& outInfo)
{
    memset(&outInfo, 0, sizeof(outInfo));
    outInfo.SuballocationSizeMin = UINT64_MAX;
    outInfo.UnusedRangeSizeMin = UINT64_MAX;
}

static void CalcAllocationStatInfo(VmaStatInfo& outInfo, const VmaAllocation& alloc)
{
    outInfo.AllocationCount = 1;

    const uint32_t rangeCount = (uint32_t)alloc.m_Suballocations.size();
    outInfo.SuballocationCount = rangeCount - alloc.m_FreeCount;
    outInfo.UnusedRangeCount = alloc.m_FreeCount;
    
    outInfo.UnusedBytes = alloc.m_SumFreeSize;
    outInfo.UsedBytes = alloc.m_Size - outInfo.UnusedBytes;

    outInfo.SuballocationSizeMin = UINT64_MAX;
    outInfo.SuballocationSizeMax = 0;
    outInfo.UnusedRangeSizeMin = UINT64_MAX;
    outInfo.UnusedRangeSizeMax = 0;

    for(VmaSuballocationList::const_iterator suballocItem = alloc.m_Suballocations.cbegin();
        suballocItem != alloc.m_Suballocations.cend();
        ++suballocItem)
    {
        const VmaSuballocation& suballoc = *suballocItem;
        if(suballoc.type != VMA_SUBALLOCATION_TYPE_FREE)
        {
            outInfo.SuballocationSizeMin = VMA_MIN(outInfo.SuballocationSizeMin, suballoc.size);
            outInfo.SuballocationSizeMax = VMA_MAX(outInfo.SuballocationSizeMax, suballoc.size);
        }
        else
        {
            outInfo.UnusedRangeSizeMin = VMA_MIN(outInfo.UnusedRangeSizeMin, suballoc.size);
            outInfo.UnusedRangeSizeMax = VMA_MAX(outInfo.UnusedRangeSizeMax, suballoc.size);
        }
    }
}

// Adds statistics srcInfo into inoutInfo, like: inoutInfo += srcInfo.
static void VmaAddStatInfo(VmaStatInfo& inoutInfo, const VmaStatInfo& srcInfo)
{
    inoutInfo.AllocationCount += srcInfo.AllocationCount;
    inoutInfo.SuballocationCount += srcInfo.SuballocationCount;
    inoutInfo.UnusedRangeCount += srcInfo.UnusedRangeCount;
    inoutInfo.UsedBytes += srcInfo.UsedBytes;
    inoutInfo.UnusedBytes += srcInfo.UnusedBytes;
    inoutInfo.SuballocationSizeMin = VMA_MIN(inoutInfo.SuballocationSizeMin, srcInfo.SuballocationSizeMin);
    inoutInfo.SuballocationSizeMax = VMA_MAX(inoutInfo.SuballocationSizeMax, srcInfo.SuballocationSizeMax);
    inoutInfo.UnusedRangeSizeMin = VMA_MIN(inoutInfo.UnusedRangeSizeMin, srcInfo.UnusedRangeSizeMin);
    inoutInfo.UnusedRangeSizeMax = VMA_MAX(inoutInfo.UnusedRangeSizeMax, srcInfo.UnusedRangeSizeMax);
}

static void VmaPostprocessCalcStatInfo(VmaStatInfo& inoutInfo)
{
    inoutInfo.SuballocationSizeAvg = (inoutInfo.SuballocationCount > 0) ?
        VmaRoundDiv<VkDeviceSize>(inoutInfo.UsedBytes, inoutInfo.SuballocationCount) : 0;
    inoutInfo.UnusedRangeSizeAvg = (inoutInfo.UnusedRangeCount > 0) ?
        VmaRoundDiv<VkDeviceSize>(inoutInfo.UnusedBytes, inoutInfo.UnusedRangeCount) : 0;
}

VmaAllocationVector::VmaAllocationVector(VmaAllocator hAllocator) :
    m_hAllocator(hAllocator),
    m_Allocations(VmaStlAllocator<VmaAllocation*>(hAllocator->GetAllocationCallbacks()))
{
}

VmaAllocationVector::~VmaAllocationVector()
{
    for(size_t i = m_Allocations.size(); i--; )
    {
        m_Allocations[i]->Destroy(m_hAllocator);
        vma_delete(m_hAllocator, m_Allocations[i]);
    }
}

size_t VmaAllocationVector::Free(const VkMappedMemoryRange* pMemory)
{
    for(uint32_t allocIndex = 0; allocIndex < m_Allocations.size(); ++allocIndex)
    {
        VmaAllocation* const pAlloc = m_Allocations[allocIndex];
        VMA_ASSERT(pAlloc);
        if(pAlloc->m_hMemory == pMemory->memory)
        {
            pAlloc->Free(pMemory);
            VMA_HEAVY_ASSERT(pAlloc->Validate());
            return allocIndex;
        }
    }

    return (size_t)-1;
}

void VmaAllocationVector::IncrementallySortAllocations()
{
    // Bubble sort only until first swap.
    for(size_t i = 1; i < m_Allocations.size(); ++i)
    {
        if(m_Allocations[i - 1]->m_SumFreeSize > m_Allocations[i]->m_SumFreeSize)
        {
            VMA_SWAP(m_Allocations[i - 1], m_Allocations[i]);
            return;
        }
    }
}

#if VMA_STATS_STRING_ENABLED

void VmaAllocationVector::PrintDetailedMap(class VmaStringBuilder& sb) const
{
    for(size_t i = 0; i < m_Allocations.size(); ++i)
    {
        if(i > 0)
            sb.Add(",\n\t\t");
        else
            sb.Add("\n\t\t");
        m_Allocations[i]->PrintDetailedMap(sb);
    }
}

#endif // #if VMA_STATS_STRING_ENABLED

void VmaAllocationVector::AddStats(VmaStats* pStats, uint32_t memTypeIndex, uint32_t memHeapIndex) const
{
    for(uint32_t allocIndex = 0; allocIndex < m_Allocations.size(); ++allocIndex)
    {
        const VmaAllocation* const pAlloc = m_Allocations[allocIndex];
        VMA_ASSERT(pAlloc);
        VMA_HEAVY_ASSERT(pAlloc->Validate());
        VmaStatInfo allocationStatInfo;
        CalcAllocationStatInfo(allocationStatInfo, *pAlloc);
        VmaAddStatInfo(pStats->total, allocationStatInfo);
        VmaAddStatInfo(pStats->memoryType[memTypeIndex], allocationStatInfo);
        VmaAddStatInfo(pStats->memoryHeap[memHeapIndex], allocationStatInfo);
    }
}

////////////////////////////////////////////////////////////////////////////////
// VmaAllocator_T

VmaAllocator_T::VmaAllocator_T(const VmaAllocatorCreateInfo* pCreateInfo) :
    m_PhysicalDevice(pCreateInfo->physicalDevice),
    m_hDevice(pCreateInfo->device),
    m_AllocationCallbacksSpecified(pCreateInfo->pAllocationCallbacks != VMA_NULL),
    m_AllocationCallbacks(pCreateInfo->pAllocationCallbacks ?
        *pCreateInfo->pAllocationCallbacks : VmaEmptyAllocationCallbacks),
    m_PreferredLargeHeapBlockSize(0),
    m_PreferredSmallHeapBlockSize(0),
    m_BufferToMemoryMap(VmaStlAllocator< VmaPair<VkBuffer, VkMappedMemoryRange> >(pCreateInfo->pAllocationCallbacks)),
    m_ImageToMemoryMap(VmaStlAllocator< VmaPair<VkImage, VkMappedMemoryRange> >(pCreateInfo->pAllocationCallbacks))
{
    VMA_ASSERT(pCreateInfo->physicalDevice && pCreateInfo->device);    

    memset(&m_MemProps, 0, sizeof(m_MemProps));
    memset(&m_PhysicalDeviceProperties, 0, sizeof(m_PhysicalDeviceProperties));
        
    memset(&m_pAllocations, 0, sizeof(m_pAllocations));
    memset(&m_HasEmptyAllocation, 0, sizeof(m_HasEmptyAllocation));
    memset(&m_pOwnAllocations, 0, sizeof(m_pOwnAllocations));

    m_PreferredLargeHeapBlockSize = (pCreateInfo->preferredLargeHeapBlockSize != 0) ?
        pCreateInfo->preferredLargeHeapBlockSize : static_cast<VkDeviceSize>(VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE);
    m_PreferredSmallHeapBlockSize = (pCreateInfo->preferredSmallHeapBlockSize != 0) ?
        pCreateInfo->preferredSmallHeapBlockSize : static_cast<VkDeviceSize>(VMA_DEFAULT_SMALL_HEAP_BLOCK_SIZE);

    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemProps);

    for(size_t i = 0; i < GetMemoryTypeCount(); ++i)
    {
        m_pAllocations[i] = vma_new(this, VmaAllocationVector)(this);
        m_pOwnAllocations[i] = vma_new(this, OwnAllocationVectorType)(VmaStlAllocator<VmaOwnAllocation>(GetAllocationCallbacks()));
    }
}

VmaAllocator_T::~VmaAllocator_T()
{
    for(VMA_MAP_TYPE(VkImage, VkMappedMemoryRange)::iterator it = m_ImageToMemoryMap.begin();
        it != m_ImageToMemoryMap.end();
        ++it)
    {
        vkDestroyImage(m_hDevice, it->first, GetAllocationCallbacks());
    }

    for(VMA_MAP_TYPE(VkBuffer, VkMappedMemoryRange)::iterator it = m_BufferToMemoryMap.begin();
        it != m_BufferToMemoryMap.end();
        ++it)
    {
        vkDestroyBuffer(m_hDevice, it->first, GetAllocationCallbacks());
    }

    for(uint32_t typeIndex = 0; typeIndex < GetMemoryTypeCount(); ++typeIndex)
    {
        OwnAllocationVectorType* pOwnAllocations = m_pOwnAllocations[typeIndex];
        VMA_ASSERT(pOwnAllocations);
        for(size_t allocationIndex = 0; allocationIndex < pOwnAllocations->size(); ++allocationIndex)
        {
            const VmaOwnAllocation& ownAlloc = (*pOwnAllocations)[allocationIndex];
            vkFreeMemory(m_hDevice, ownAlloc.m_hMemory, GetAllocationCallbacks());
        }
    }

    for(size_t i = GetMemoryTypeCount(); i--; )
    {
        vma_delete(this, m_pAllocations[i]);
        vma_delete(this, m_pOwnAllocations[i]);
    }
}

VkDeviceSize VmaAllocator_T::GetPreferredBlockSize(uint32_t memTypeIndex) const
{
    VkDeviceSize heapSize = m_MemProps.memoryHeaps[m_MemProps.memoryTypes[memTypeIndex].heapIndex].size;
    return (heapSize <= VMA_SMALL_HEAP_MAX_SIZE) ?
        m_PreferredSmallHeapBlockSize : m_PreferredLargeHeapBlockSize;
}

VkResult VmaAllocator_T::AllocateMemoryOfType(
    const VkMemoryRequirements& vkMemReq,
    const VmaMemoryRequirements& vmaMemReq,
    uint32_t memTypeIndex,
    VmaSuballocationType suballocType,
    VkMappedMemoryRange* pMemory)
{
    VMA_DEBUG_LOG("  AllocateMemory: MemoryTypeIndex=%u, Size=%llu", memTypeIndex, vkMemReq.size);

    pMemory->sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    pMemory->pNext = VMA_NULL;
    pMemory->size = vkMemReq.size;

    const VkDeviceSize preferredBlockSize = GetPreferredBlockSize(memTypeIndex);
    // Heuristics: Allocate own memory if requested size if greater than half of preferred block size.
    const bool ownMemory =
        vmaMemReq.ownMemory ||
        VMA_DEBUG_ALWAYS_OWN_MEMORY ||
        ((vmaMemReq.neverAllocate == false) && (vkMemReq.size > preferredBlockSize / 2));

    if(ownMemory)
    {
        if(vmaMemReq.neverAllocate)
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        else
            return AllocateOwnMemory(vkMemReq.size, suballocType, memTypeIndex, pMemory);
    }
    else
    {
        VmaMutexLock lock(m_AllocationsMutex[memTypeIndex]);
        VmaAllocationVector* const allocationVector = m_pAllocations[memTypeIndex];
        VMA_ASSERT(allocationVector);

        // 1. Search existing allocations.
        // Forward order - prefer blocks with smallest amount of free space.
        for(size_t allocIndex = 0; allocIndex < allocationVector->m_Allocations.size(); ++allocIndex )
        {
            VmaAllocation* const pAlloc = allocationVector->m_Allocations[allocIndex];
            VMA_ASSERT(pAlloc);
            VmaAllocationRequest allocRequest = {};
            // Check if can allocate from pAlloc.
            if(pAlloc->CreateAllocationRequest(
                GetBufferImageGranularity(),
                vkMemReq.size,
                vkMemReq.alignment,
                suballocType,
                &allocRequest))
            {
                // We no longer have an empty Allocation.
                if(pAlloc->IsEmpty())
                    m_HasEmptyAllocation[memTypeIndex] = false;
                // Allocate from this pAlloc.
                pAlloc->Alloc(allocRequest, suballocType, vkMemReq.size);
                // Return VkDeviceMemory and offset (size already filled above).
                pMemory->memory = pAlloc->m_hMemory;
                pMemory->offset = allocRequest.offset;
                VMA_HEAVY_ASSERT(pAlloc->Validate());
                VMA_DEBUG_LOG("    Returned from existing allocation #%u", (uint32_t)allocIndex);
                return VK_SUCCESS;
            }
        }

        // 2. Create new Allocation.
        if(vmaMemReq.neverAllocate)
        {
            VMA_DEBUG_LOG("    FAILED due to VmaMemoryRequirements::neverAllocate");
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        }
        else
        {
            // Start with full preferredBlockSize.
            VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
            allocInfo.memoryTypeIndex = memTypeIndex;
            allocInfo.allocationSize = preferredBlockSize;
            VkDeviceMemory mem = VK_NULL_HANDLE;
            VkResult res = vkAllocateMemory(m_hDevice, &allocInfo, GetAllocationCallbacks(), &mem);
            if(res < 0)
            {
                // 3. Try half the size.
                allocInfo.allocationSize /= 2;
                if(allocInfo.allocationSize >= vkMemReq.size)
                {
                    res = vkAllocateMemory(m_hDevice, &allocInfo, GetAllocationCallbacks(), &mem);
                    if(res < 0)
                    {
                        // 4. Try quarter the size.
                        allocInfo.allocationSize /= 2;
                        if(allocInfo.allocationSize >= vkMemReq.size)
                        {
                            res = vkAllocateMemory(m_hDevice, &allocInfo, GetAllocationCallbacks(), &mem);
                        }
                    }
                }
            }
            if(res < 0)
            {
                // 5. Try OwnAlloc.
                res = AllocateOwnMemory(vkMemReq.size, suballocType, memTypeIndex, pMemory);
                if(res == VK_SUCCESS)
                {
                    // Succeeded: AllocateOwnMemory function already filld pMemory, nothing more to do here.
                    VMA_DEBUG_LOG("    Allocated as OwnMemory");
                    return VK_SUCCESS;
                }
                else
                {
                    // Everything failed: Return error code.
                    VMA_DEBUG_LOG("    vkAllocateMemory FAILED");
                    return res;
                }
            }

            // New VkDeviceMemory successfully created. Create new Allocation for it.
            VmaAllocation* const pAlloc = vma_new(this, VmaAllocation)(this);
            pAlloc->Init(mem, allocInfo.allocationSize);

            allocationVector->m_Allocations.push_back(pAlloc);

            // Allocate from pAlloc. Because it is empty, allocRequest can be trivially filled.
            VmaAllocationRequest allocRequest = {};
            allocRequest.freeSuballocationItem = pAlloc->m_Suballocations.begin();
            allocRequest.offset = 0;
            pAlloc->Alloc(allocRequest, suballocType, vkMemReq.size);
            pMemory->memory = mem;
            pMemory->offset = allocRequest.offset;
            VMA_HEAVY_ASSERT(pAlloc->Validate());
            VMA_DEBUG_LOG("    Created new allocation Size=%llu", allocInfo.allocationSize);
            return VK_SUCCESS;
        }
    }
}

VkResult VmaAllocator_T::AllocateOwnMemory(
    VkDeviceSize size,
    VmaSuballocationType suballocType,
    uint32_t memTypeIndex,
    VkMappedMemoryRange* pMemory)
{
    VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.memoryTypeIndex = memTypeIndex;
    allocInfo.allocationSize = size;

    // Allocate VkDeviceMemory.
    VmaOwnAllocation ownAlloc = {};
    ownAlloc.m_Size = size;
    ownAlloc.m_Type = suballocType;
    VkResult res = vkAllocateMemory(m_hDevice, &allocInfo, GetAllocationCallbacks(), &ownAlloc.m_hMemory);
    if(res < 0)
    {
        VMA_DEBUG_LOG("    vkAllocateMemory FAILED");
        return res;
    }

    // Register it in m_pOwnAllocations.
    VmaMutexLock lock(m_OwnAllocationsMutex[memTypeIndex]);
    OwnAllocationVectorType* ownAllocations = m_pOwnAllocations[memTypeIndex];
    VMA_ASSERT(ownAllocations);
    VmaOwnAllocation* const pOwnAllocationsBeg = ownAllocations->data();
    VmaOwnAllocation* const pOwnAllocationsEnd = pOwnAllocationsBeg + ownAllocations->size();
    const size_t indexToInsert = VmaBinaryFindFirstNotLess(
        pOwnAllocationsBeg,
        pOwnAllocationsEnd,
        ownAlloc,
        VmaOwnAllocationMemoryHandleLess()) - pOwnAllocationsBeg;
    VectorInsert(*ownAllocations, indexToInsert, ownAlloc);

    // Return parameters of the allocation.
    pMemory->sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    pMemory->pNext = VMA_NULL;
    pMemory->memory = ownAlloc.m_hMemory;
    pMemory->offset = 0;
    pMemory->size = size;

    VMA_DEBUG_LOG("    Allocated OwnMemory MemoryTypeIndex=#%u", memTypeIndex);

    return VK_SUCCESS;
}

VkResult VmaAllocator_T::AllocateMemory(
    const VkMemoryRequirements& vkMemReq,
    const VmaMemoryRequirements& vmaMemReq,
    VmaSuballocationType suballocType,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex)
{
    if(vmaMemReq.ownMemory && vmaMemReq.neverAllocate)
    {
        VMA_ASSERT(0 && "Specifying VmaMemoryRequirements::ownMemory && VmaMemoryRequirements::neverAllocate makes no sense.");
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    // Bit mask of memory Vulkan types acceptable for this allocation.
    uint32_t memoryTypeBits = vkMemReq.memoryTypeBits;
    uint32_t memTypeIndex = UINT32_MAX;
    VkResult res = vmaFindMemoryTypeIndex(this, memoryTypeBits, &vmaMemReq, &memTypeIndex);
    if(res == VK_SUCCESS)
    {
        res = AllocateMemoryOfType(vkMemReq, vmaMemReq, memTypeIndex, suballocType, pMemory);
        // Succeeded on first try.
        if(res == VK_SUCCESS)
        {
            if(pMemoryTypeIndex != VMA_NULL)
                *pMemoryTypeIndex = memTypeIndex;
            return res;
        }
        // Allocation from this memory type failed. Try other compatible memory types.
        else
        {
            for(;;)
            {
                // Remove old memTypeIndex from list of possibilities.
                memoryTypeBits &= ~(1u << memTypeIndex);
                // Find alternative memTypeIndex.
                res = vmaFindMemoryTypeIndex(this, memoryTypeBits, &vmaMemReq, &memTypeIndex);
                if(res == VK_SUCCESS)
                {
                    res = AllocateMemoryOfType(vkMemReq, vmaMemReq, memTypeIndex, suballocType, pMemory);
                    // Allocation from this alternative memory type succeeded.
                    if(res == VK_SUCCESS)
                    {
                        if(pMemoryTypeIndex != VMA_NULL)
                            *pMemoryTypeIndex = memTypeIndex;
                        return res;
                    }
                    // else: Allocation from this memory type failed. Try next one - next loop iteration.
                }
                // No other matching memory type index could be found.
                else
                    // Not returning res, which is VK_ERROR_FEATURE_NOT_PRESENT, because we already failed to allocate once.
                    return VK_ERROR_OUT_OF_DEVICE_MEMORY;
            }
        }
    }
    // Can't find any single memory type maching requirements. res is VK_ERROR_FEATURE_NOT_PRESENT.
    else
        return res;
}

void VmaAllocator_T::FreeMemory(const VkMappedMemoryRange* pMemory)
{
    uint32_t memTypeIndex = 0;
    bool found = false;
    VmaAllocation* allocationToDelete = VMA_NULL;
    // Check all memory types because we don't know which one does pMemory come from.
    for(; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex)
    {
        VmaMutexLock lock(m_AllocationsMutex[memTypeIndex]);
        VmaAllocationVector* const pAllocationVector = m_pAllocations[memTypeIndex];
        VMA_ASSERT(pAllocationVector);
        // Try to free pMemory from pAllocationVector.
        const size_t allocIndex = pAllocationVector->Free(pMemory);
        if(allocIndex != (size_t)-1)
        {
            VMA_DEBUG_LOG("  Freed from MemoryTypeIndex=%u", memTypeIndex);
            found = true;
            VmaAllocation* const pAlloc = pAllocationVector->m_Allocations[allocIndex];
            VMA_ASSERT(pAlloc);
            // pAlloc became empty after this deallocation.
            if(pAlloc->IsEmpty())
            {
                // Already has empty Allocation. We don't want to have two, so delete this one.
                if(m_HasEmptyAllocation[memTypeIndex])
                {
                    allocationToDelete = pAlloc;
                    VectorRemove(pAllocationVector->m_Allocations, allocIndex);
                    break;
                }
                // We now have first empty Allocation.
                else
                    m_HasEmptyAllocation[memTypeIndex] = true;
            }
            // Must be called after allocIndex is used, because later it may become invalid!
            pAllocationVector->IncrementallySortAllocations();
            break;
        }
    }
    if(found)
    {
        // Destruction of a free Allocation. Deferred until this point, outside of mutex
        // lock, for performance reason.
        if(allocationToDelete != VMA_NULL)
        {
            VMA_DEBUG_LOG("    Deleted empty allocation");
            allocationToDelete->Destroy(this);
            vma_delete(this, allocationToDelete);
        }
        return;
    }

    // pMemory not found in allocations. Try free it as Own Memory.
    if(FreeOwnMemory(pMemory))
        return;

    // pMemory not found as Own Memory either.
    VMA_ASSERT(0 && "Not found. Trying to free memory not allocated using this allocator (or some other bug).");
}

void VmaAllocator_T::CalculateStats(VmaStats* pStats)
{
    InitStatInfo(pStats->total);
    for(size_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i)
        InitStatInfo(pStats->memoryType[i]);
    for(size_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i)
        InitStatInfo(pStats->memoryHeap[i]);
    
    for(uint32_t memTypeIndex = 0; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex)
    {
        VmaMutexLock allocationsLock(m_AllocationsMutex[memTypeIndex]);
        const uint32_t heapIndex = m_MemProps.memoryTypes[memTypeIndex].heapIndex;
        const VmaAllocationVector* const allocVector = m_pAllocations[memTypeIndex];
        VMA_ASSERT(allocVector);
        allocVector->AddStats(pStats, memTypeIndex, heapIndex);
    }

    VmaPostprocessCalcStatInfo(pStats->total);
    for(size_t i = 0; i < GetMemoryTypeCount(); ++i)
        VmaPostprocessCalcStatInfo(pStats->memoryType[i]);
    for(size_t i = 0; i < GetMemoryHeapCount(); ++i)
        VmaPostprocessCalcStatInfo(pStats->memoryHeap[i]);
}

bool VmaAllocator_T::FreeOwnMemory(const VkMappedMemoryRange* pMemory)
{
    VkDeviceMemory vkMemory = VK_NULL_HANDLE;

    // Check all memory types because we don't know which one does pMemory come from.
    for(uint32_t memTypeIndex = 0; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex)
    {
        VmaMutexLock lock(m_OwnAllocationsMutex[memTypeIndex]);
        OwnAllocationVectorType* const pOwnAllocations = m_pOwnAllocations[memTypeIndex];
        VMA_ASSERT(pOwnAllocations);
        VmaOwnAllocation* const pOwnAllocationsBeg = pOwnAllocations->data();
        VmaOwnAllocation* const pOwnAllocationsEnd = pOwnAllocationsBeg + pOwnAllocations->size();
        VmaOwnAllocation* const pOwnAllocationIt = VmaBinaryFindFirstNotLess(
            pOwnAllocationsBeg,
            pOwnAllocationsEnd,
            pMemory->memory,
            VmaOwnAllocationMemoryHandleLess());
        if((pOwnAllocationIt != pOwnAllocationsEnd) &&
            (pOwnAllocationIt->m_hMemory == pMemory->memory))
        {
            VMA_ASSERT(pMemory->size == pOwnAllocationIt->m_Size && pMemory->offset == 0);
            vkMemory = pOwnAllocationIt->m_hMemory;
            const size_t ownAllocationIndex = pOwnAllocationIt - pOwnAllocationsBeg;
            VectorRemove(*pOwnAllocations, ownAllocationIndex);
            VMA_DEBUG_LOG("    Freed OwnMemory MemoryTypeIndex=%u", memTypeIndex);
            break;
        }
    }

    // Found. Free VkDeviceMemory deferred until this point, outside of mutex lock,
    // for performance reason.
    if(vkMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(m_hDevice, vkMemory, GetAllocationCallbacks());
        return true;
    }
    else
        return false;
}

#if VMA_STATS_STRING_ENABLED

void VmaAllocator_T::PrintDetailedMap(VmaStringBuilder& sb)
{
    bool ownAllocationsStarted = false;
    for(size_t memTypeIndex = 0; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex)
    {
        VmaMutexLock ownAllocationsLock(m_OwnAllocationsMutex[memTypeIndex]);
        OwnAllocationVectorType* const pOwnAllocVector = m_pOwnAllocations[memTypeIndex];
        VMA_ASSERT(pOwnAllocVector);
        if(pOwnAllocVector->empty() == false)
        {
            if(ownAllocationsStarted)
                sb.Add(",\n\t\"Type ");
            else
            {
                sb.Add(",\n\"OwnAllocations\": {\n\t\"Type ");
                ownAllocationsStarted = true;
            }
            sb.AddNumber(memTypeIndex);
            sb.Add("\": [");

            for(size_t i = 0; i < pOwnAllocVector->size(); ++i)
            {
                const VmaOwnAllocation& ownAlloc = (*pOwnAllocVector)[i];
                if(i > 0)
                    sb.Add(",\n\t\t{ \"Size\": ");
                else
                    sb.Add("\n\t\t{ \"Size\": ");
                sb.AddNumber(ownAlloc.m_Size);
                sb.Add(", \"Type\": ");
                sb.AddString(VMA_SUBALLOCATION_TYPE_NAMES[ownAlloc.m_Type]);
                sb.Add(" }");
            }

            sb.Add("\n\t]");
        }
    }
    if(ownAllocationsStarted)
        sb.Add("\n}");

    {
        bool allocationsStarted = false;
        for(size_t memTypeIndex = 0; memTypeIndex < GetMemoryTypeCount(); ++memTypeIndex)
        {
            VmaMutexLock globalAllocationsLock(m_AllocationsMutex[memTypeIndex]);
            if(m_pAllocations[memTypeIndex]->IsEmpty() == false)
            {
                if(allocationsStarted)
                    sb.Add(",\n\t\"Type ");
                else
                {
                    sb.Add(",\n\"Allocations\": {\n\t\"Type ");
                    allocationsStarted = true;
                }
                sb.AddNumber(memTypeIndex);
                sb.Add("\": [");

                m_pAllocations[memTypeIndex]->PrintDetailedMap(sb);

                sb.Add("\n\t]");
            }
        }
        if(allocationsStarted)
            sb.Add("\n}");
    }
}

#endif // #if VMA_STATS_STRING_ENABLED

static VkResult AllocateMemoryForImage(
    VmaAllocator allocator,
    VkImage image,
    const VmaMemoryRequirements* pMemoryRequirements,
    VmaSuballocationType suballocType,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex)
{
    VMA_ASSERT(allocator && image != VK_NULL_HANDLE && pMemoryRequirements && pMemory);
    
    VkMemoryRequirements vkMemReq = {};
    vkGetImageMemoryRequirements(allocator->m_hDevice, image, &vkMemReq);

    return allocator->AllocateMemory(
        vkMemReq,
        *pMemoryRequirements,
        suballocType,
        pMemory,
        pMemoryTypeIndex);
}

////////////////////////////////////////////////////////////////////////////////
// Public interface

VkResult vmaCreateAllocator(
    const VmaAllocatorCreateInfo* pCreateInfo,
    VmaAllocator* pAllocator)
{
    VMA_ASSERT(pCreateInfo && pAllocator);
    VMA_DEBUG_LOG("vmaCreateAllocator");
    *pAllocator = vma_new(pCreateInfo->pAllocationCallbacks, VmaAllocator_T)(pCreateInfo);
    return VK_SUCCESS;
}

void vmaDestroyAllocator(
    VmaAllocator allocator)
{
    if(allocator != VK_NULL_HANDLE)
    {
        VMA_DEBUG_LOG("vmaDestroyAllocator");
        VkAllocationCallbacks allocationCallbacks = allocator->m_AllocationCallbacks;
        vma_delete(&allocationCallbacks, allocator);
    }
}

void vmaGetPhysicalDeviceProperties(
    VmaAllocator allocator,
    const VkPhysicalDeviceProperties **ppPhysicalDeviceProperties)
{
    VMA_ASSERT(allocator && ppPhysicalDeviceProperties);
    *ppPhysicalDeviceProperties = &allocator->m_PhysicalDeviceProperties;
}

void vmaGetMemoryProperties(
    VmaAllocator allocator,
    const VkPhysicalDeviceMemoryProperties** ppPhysicalDeviceMemoryProperties)
{
    VMA_ASSERT(allocator && ppPhysicalDeviceMemoryProperties);
    *ppPhysicalDeviceMemoryProperties = &allocator->m_MemProps;
}

void vmaGetMemoryTypeProperties(
    VmaAllocator allocator,
    uint32_t memoryTypeIndex,
    VkMemoryPropertyFlags* pFlags)
{
    VMA_ASSERT(allocator && pFlags);
    VMA_ASSERT(memoryTypeIndex < allocator->GetMemoryTypeCount());
    *pFlags = allocator->m_MemProps.memoryTypes[memoryTypeIndex].propertyFlags;
}

void vmaCalculateStats(
    VmaAllocator allocator,
    VmaStats* pStats)
{
    VMA_ASSERT(allocator && pStats);
    VMA_DEBUG_GLOBAL_MUTEX_LOCK
    allocator->CalculateStats(pStats);
}

#if VMA_STATS_STRING_ENABLED

void vmaBuildStatsString(
    VmaAllocator allocator,
    char** ppStatsString,
    VkBool32 detailedMap)
{
    VMA_ASSERT(allocator && ppStatsString);
    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    VmaStringBuilder sb(allocator);
    {
        VmaStats stats;
        allocator->CalculateStats(&stats);

        sb.Add("{\n\"Total\": ");
        VmaPrintStatInfo(sb, stats.total);
    
        for(uint32_t heapIndex = 0; heapIndex < allocator->GetMemoryHeapCount(); ++heapIndex)
        {
            sb.Add(",\n\"Heap ");
            sb.AddNumber(heapIndex);
            sb.Add("\": {\n\t\"Size\": ");
            sb.AddNumber(allocator->m_MemProps.memoryHeaps[heapIndex].size);
            sb.Add(",\n\t\"Flags\": ");
            if((allocator->m_MemProps.memoryHeaps[heapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)
                sb.AddString("DEVICE_LOCAL");
            else
                sb.AddString("");
            if(stats.memoryHeap[heapIndex].AllocationCount > 0)
            {
                sb.Add(",\n\t\"Stats:\": ");
                VmaPrintStatInfo(sb, stats.memoryHeap[heapIndex]);
            }

            for(uint32_t typeIndex = 0; typeIndex < allocator->GetMemoryTypeCount(); ++typeIndex)
            {
                if(allocator->m_MemProps.memoryTypes[typeIndex].heapIndex == heapIndex)
                {
                    sb.Add(",\n\t\"Type ");
                    sb.AddNumber(typeIndex);
                    sb.Add("\": {\n\t\t\"Flags\": \"");
                    VkMemoryPropertyFlags flags = allocator->m_MemProps.memoryTypes[typeIndex].propertyFlags;
                    if((flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)
                        sb.Add(" DEVICE_LOCAL");
                    if((flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
                        sb.Add(" HOST_VISIBLE");
                    if((flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0)
                        sb.Add(" HOST_COHERENT");
                    if((flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0)
                        sb.Add(" HOST_CACHED");
                    if((flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0)
                        sb.Add(" LAZILY_ALLOCATED");
                    sb.Add("\"");
                    if(stats.memoryType[typeIndex].AllocationCount > 0)
                    {
                        sb.Add(",\n\t\t\"Stats\": ");
                        VmaPrintStatInfo(sb, stats.memoryType[typeIndex]);
                    }
                    sb.Add("\n\t}");
                }
            }
            sb.Add("\n}");
        }
        if(detailedMap == VK_TRUE)
            allocator->PrintDetailedMap(sb);
        sb.Add("\n}\n");
    }

    const size_t len = sb.GetLength();
    char* const pChars = vma_new_array(allocator, char, len + 1);
    if(len > 0)
        memcpy(pChars, sb.GetData(), len);
    pChars[len] = '\0';
    *ppStatsString = pChars;
}

void vmaFreeStatsString(
    VmaAllocator allocator,
    char* pStatsString)
{
    if(pStatsString != VMA_NULL)
    {
        VMA_ASSERT(allocator);
        size_t len = strlen(pStatsString);
        vma_delete_array(allocator, pStatsString, len + 1);
    }
}

#endif // #if VMA_STATS_STRING_ENABLED

/** This function is not protected by any mutex because it just reads immutable data.
*/
VkResult vmaFindMemoryTypeIndex(
    VmaAllocator allocator,
    uint32_t memoryTypeBits,
    const VmaMemoryRequirements* pMemoryRequirements,
    uint32_t* pMemoryTypeIndex)
{
    VMA_ASSERT(allocator != VK_NULL_HANDLE);
    VMA_ASSERT(pMemoryRequirements != VMA_NULL);
    VMA_ASSERT(pMemoryTypeIndex != VMA_NULL);
    
    uint32_t requiredFlags = pMemoryRequirements->requiredFlags;
    uint32_t preferredFlags = pMemoryRequirements->preferredFlags;
    if(preferredFlags == 0)
        preferredFlags = requiredFlags;
    // preferredFlags, if not 0, must be a superset of requiredFlags.
    VMA_ASSERT((requiredFlags & ~preferredFlags) == 0);

    // Convert usage to requiredFlags and preferredFlags.
    switch(pMemoryRequirements->usage)
    {
    case VMA_MEMORY_USAGE_UNKNOWN:
        break;
    case VMA_MEMORY_USAGE_GPU_ONLY:
        preferredFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    case VMA_MEMORY_USAGE_CPU_ONLY:
        requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        break;
    case VMA_MEMORY_USAGE_CPU_TO_GPU:
        requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        preferredFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    case VMA_MEMORY_USAGE_GPU_TO_CPU:
        requiredFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        preferredFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        break;
    default:
        break;
    }

    *pMemoryTypeIndex = UINT32_MAX;
    uint32_t minCost = UINT32_MAX;
    for(uint32_t memTypeIndex = 0, memTypeBit = 1;
        memTypeIndex < allocator->GetMemoryTypeCount();
        ++memTypeIndex, memTypeBit <<= 1)
    {
        // This memory type is acceptable according to memoryTypeBits bitmask.
        if((memTypeBit & memoryTypeBits) != 0)
        {
            const VkMemoryPropertyFlags currFlags =
                allocator->m_MemProps.memoryTypes[memTypeIndex].propertyFlags;
            // This memory type contains requiredFlags.
            if((requiredFlags & ~currFlags) == 0)
            {
                // Calculate cost as number of bits from preferredFlags not present in this memory type.
                uint32_t currCost = CountBitsSet(preferredFlags & ~currFlags);
                // Remember memory type with lowest cost.
                if(currCost < minCost)
                {
                    *pMemoryTypeIndex = memTypeIndex;
                    if(currCost == 0)
                        return VK_SUCCESS;
                    minCost = currCost;
                }
            }
        }
    }
    return (*pMemoryTypeIndex != UINT32_MAX) ? VK_SUCCESS : VK_ERROR_FEATURE_NOT_PRESENT;
}

VkResult vmaAllocateMemory(
    VmaAllocator allocator,
    const VkMemoryRequirements* pVkMemoryRequirements,
    const VmaMemoryRequirements* pVmaMemoryRequirements,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex)
{
    VMA_ASSERT(allocator && pVkMemoryRequirements && pVmaMemoryRequirements && pMemory);

    VMA_DEBUG_LOG("vmaAllocateMemory");

    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    return allocator->AllocateMemory(
        *pVkMemoryRequirements,
        *pVmaMemoryRequirements,
        VMA_SUBALLOCATION_TYPE_UNKNOWN,
        pMemory,
        pMemoryTypeIndex);
}

VkResult vmaAllocateMemoryForBuffer(
    VmaAllocator allocator,
    VkBuffer buffer,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex)
{
    VMA_ASSERT(allocator && buffer != VK_NULL_HANDLE && pMemoryRequirements && pMemory);

    VMA_DEBUG_LOG("vmaAllocateMemoryForBuffer");

    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    VkMemoryRequirements vkMemReq = {};
    vkGetBufferMemoryRequirements(allocator->m_hDevice, buffer, &vkMemReq);

    return allocator->AllocateMemory(
        vkMemReq,
        *pMemoryRequirements,
        VMA_SUBALLOCATION_TYPE_BUFFER,
        pMemory,
        pMemoryTypeIndex);
}

VkResult vmaAllocateMemoryForImage(
    VmaAllocator allocator,
    VkImage image,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex)
{
    VMA_ASSERT(allocator && image != VK_NULL_HANDLE && pMemoryRequirements);

    VMA_DEBUG_LOG("vmaAllocateMemoryForImage");

    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    return AllocateMemoryForImage(
        allocator,
        image,
        pMemoryRequirements,
        VMA_SUBALLOCATION_TYPE_IMAGE_UNKNOWN,
        pMemory,
        pMemoryTypeIndex);
}

void vmaFreeMemory(
    VmaAllocator allocator,
    const VkMappedMemoryRange* pMemory)
{
    VMA_ASSERT(allocator && pMemory);

    VMA_DEBUG_LOG("vmaFreeMemory");

    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    allocator->FreeMemory(pMemory);
}

VkResult vmaMapMemory(
    VmaAllocator allocator,
    const VkMappedMemoryRange* pMemory,
    void** ppData)
{
    VMA_ASSERT(allocator && pMemory && ppData);

    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    return vkMapMemory(allocator->m_hDevice, pMemory->memory,
        pMemory->offset, pMemory->size, 0, ppData);
}

void vmaUnmapMemory(
    VmaAllocator allocator,
    const VkMappedMemoryRange* pMemory)
{
    VMA_ASSERT(allocator && pMemory);

    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    vkUnmapMemory(allocator->m_hDevice, pMemory->memory);
}

VkResult vmaCreateBuffer(
    VmaAllocator allocator,
    const VkBufferCreateInfo* pCreateInfo,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkBuffer* pBuffer,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex)
{
    VMA_ASSERT(allocator && pCreateInfo && pMemoryRequirements);
    
    VMA_DEBUG_LOG("vmaCreateBuffer");
    
    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    // 1. Create VkBuffer.
    VkResult res = vkCreateBuffer(allocator->m_hDevice, pCreateInfo, allocator->GetAllocationCallbacks(), pBuffer);
    if(res >= 0)
    {
        VkMappedMemoryRange mem = {};

        // 2. vkGetBufferMemoryRequirements.
        VkMemoryRequirements vkMemReq = {};
        vkGetBufferMemoryRequirements(allocator->m_hDevice, *pBuffer, &vkMemReq);

        // 3. Allocate memory using allocator.
        res = allocator->AllocateMemory(
            vkMemReq,
            *pMemoryRequirements,
            VMA_SUBALLOCATION_TYPE_BUFFER,
            &mem,
            pMemoryTypeIndex);
        if(res >= 0)
        {
            if(pMemory != VMA_NULL)
            {
                *pMemory = mem;
            }
            // 3. Bind buffer with memory.
            res = vkBindBufferMemory(allocator->m_hDevice, *pBuffer, mem.memory, mem.offset);
            if(res >= 0)
            {
                // All steps succeeded.
                VmaMutexLock lock(allocator->m_BufferToMemoryMapMutex);
                allocator->m_BufferToMemoryMap.insert(VmaPair<VkBuffer, VkMappedMemoryRange>(*pBuffer, mem));
                return VK_SUCCESS;
            }
            allocator->FreeMemory(&mem);
            return res;
        }
        vkDestroyBuffer(allocator->m_hDevice, *pBuffer, allocator->GetAllocationCallbacks());
        return res;
    }
    return res;
}

void vmaDestroyBuffer(
    VmaAllocator allocator,
    VkBuffer buffer)
{
    if(buffer != VK_NULL_HANDLE)
    {
        VMA_ASSERT(allocator);

        VMA_DEBUG_LOG("vmaDestroyBuffer");

        VMA_DEBUG_GLOBAL_MUTEX_LOCK

        VkMappedMemoryRange mem = {};
        {
            VmaMutexLock lock(allocator->m_BufferToMemoryMapMutex);
            VMA_MAP_TYPE(VkBuffer, VkMappedMemoryRange)::iterator it = allocator->m_BufferToMemoryMap.find(buffer);
            if(it == allocator->m_BufferToMemoryMap.end())
            {
                VMA_ASSERT(0 && "Trying to destroy buffer that was not created using vmaCreateBuffer or already freed.");
                return;
            }
            mem = it->second;
            allocator->m_BufferToMemoryMap.erase(it);
        }

        vkDestroyBuffer(allocator->m_hDevice, buffer, allocator->GetAllocationCallbacks());
        
        allocator->FreeMemory(&mem);
    }
}

VkResult vmaCreateImage(
    VmaAllocator allocator,
    const VkImageCreateInfo* pCreateInfo,
    const VmaMemoryRequirements* pMemoryRequirements,
    VkImage* pImage,
    VkMappedMemoryRange* pMemory,
    uint32_t* pMemoryTypeIndex)
{
    VMA_ASSERT(allocator && pCreateInfo && pMemoryRequirements);

    VMA_DEBUG_LOG("vmaCreateImage");

    VMA_DEBUG_GLOBAL_MUTEX_LOCK

    // 1. Create VkImage.
    VkResult res = vkCreateImage(allocator->m_hDevice, pCreateInfo, allocator->GetAllocationCallbacks(), pImage);
    if(res >= 0)
    {
        VkMappedMemoryRange mem = {};
        VmaSuballocationType suballocType = pCreateInfo->tiling == VK_IMAGE_TILING_OPTIMAL ?
            VMA_SUBALLOCATION_TYPE_IMAGE_OPTIMAL :
            VMA_SUBALLOCATION_TYPE_IMAGE_LINEAR;
        
        // 2. Allocate memory using allocator.
        res = AllocateMemoryForImage(allocator, *pImage, pMemoryRequirements, suballocType, &mem, pMemoryTypeIndex);
        if(res >= 0)
        {
            if(pMemory != VMA_NULL)
                *pMemory = mem;
            // 3. Bind image with memory.
            res = vkBindImageMemory(allocator->m_hDevice, *pImage, mem.memory, mem.offset);
            if(res >= 0)
            {
                // All steps succeeded.
                VmaMutexLock lock(allocator->m_ImageToMemoryMapMutex);
                allocator->m_ImageToMemoryMap.insert(VmaPair<VkImage, VkMappedMemoryRange>(*pImage, mem));
                return VK_SUCCESS;
            }
            allocator->FreeMemory(&mem);
            return res;
        }
        vkDestroyImage(allocator->m_hDevice, *pImage, allocator->GetAllocationCallbacks());
        return res;
    }
    return res;
}

void vmaDestroyImage(
    VmaAllocator allocator,
    VkImage image)
{
    if(image != VK_NULL_HANDLE)
    {
        VMA_ASSERT(allocator);

        VMA_DEBUG_LOG("vmaDestroyImage");

        VMA_DEBUG_GLOBAL_MUTEX_LOCK

        VkMappedMemoryRange mem = {};
        {
            VmaMutexLock lock(allocator->m_ImageToMemoryMapMutex);
            VMA_MAP_TYPE(VkImage, VkMappedMemoryRange)::iterator it = allocator->m_ImageToMemoryMap.find(image);
            if(it == allocator->m_ImageToMemoryMap.end())
            {
                VMA_ASSERT(0 && "Trying to destroy buffer that was not created using vmaCreateBuffer or already freed.");
                return;
            }
            mem = it->second;
            allocator->m_ImageToMemoryMap.erase(it);
        }

        vkDestroyImage(allocator->m_hDevice, image, allocator->GetAllocationCallbacks());

        allocator->FreeMemory(&mem);
    }
}

#endif // #ifdef VMA_IMPLEMENTATION

#endif // AMD_VULKAN_MEMORY_ALLOCATOR_H
