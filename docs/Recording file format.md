This is an official documentation for file format used by Vulkan Memory Allocator library
to record and replay sequence of calls to its functions.
This feature can be enabled by using `VmaAllocatorCreateInfo::pRecordSettings` structure members.
For details, see main documentation of the library.
Playback can be launched using **VmaReplay** console application.

Recording is a text file.
Line endings: Unix `'\n'`.
Character encoding: single-byte (can be ASCII or UTF-8, whaterver you use in custom strings).
Suggested file extension: **csv**. File can be processed sequentially - no random access is needed.
Each line forms a separate entry.
Each line consists of a set of values (also called columns), separated by comma `','` (hence "CSV" format - Comma Separated Values).
Number of columns is different in different lines.

# Header

First line identifies file format. It must always be:

    Vulkan Memory Allocator,Calls recording

Second line identifies format version, where first column is major version and second column is minor version.
Formats with only minor version incremented are backward compatible.
VmaReplay application supports all older versions.
Current version is:

    1,3

# Configuration

Header is followed by mandatory configuration section (min format version 1.3). It starts with line:

    Config,Begin

And ends with line:

    Config,End

Between them there can be zero or more lines with configuration options. They store values of various variables from the current environment from the time of recording, like properties and limits of Vulkan physical device, available memory heaps and types, enabled Vulkan extensions, as well macros that configure VMA internals. Supported configuration options are:

    PhysicalDevice,apiVersion,<uint32>
    PhysicalDevice,driverVersion,<uint32>
    PhysicalDevice,vendorID,<uint32>
    PhysicalDevice,deviceID,<uint32>
    PhysicalDevice,deviceType,<uint32>
    PhysicalDevice,deviceName,<string>
    
    PhysicalDeviceLimits,maxMemoryAllocationCount,<uint32>
    PhysicalDeviceLimits,bufferImageGranularity,<uint64>
    PhysicalDeviceLimits,nonCoherentAtomSize,<uint64>
    
    PhysicalDeviceMemory,HeapCount,<uint32>
    PhysicalDeviceMemory,Heap,<index:uint32>,size,<uint64>
    PhysicalDeviceMemory,Heap,<index:uint32>,flags,<uint32>
    PhysicalDeviceMemory,TypeCount,<uint32>
    PhysicalDeviceMemory,Type,<index:uint32>,heapIndex,<uint32>
    PhysicalDeviceMemory,Type,<index:uint32>,propertyFlags,<uint32>
    
    Extension,VK_KHR_dedicated_allocation,<bool>
    
    Macro,VMA_DEBUG_ALWAYS_DEDICATED_MEMORY,<bool>
    Macro,VMA_DEBUG_ALIGNMENT,<uint64>
    Macro,VMA_DEBUG_MARGIN,<uint64>
    Macro,VMA_DEBUG_INITIALIZE_ALLOCATIONS,<bool>
    Macro,VMA_DEBUG_DETECT_CORRUPTION,<bool>
    Macro,VMA_DEBUG_GLOBAL_MUTEX,<bool>
    Macro,VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY,<uint64>
    Macro,VMA_SMALL_HEAP_MAX_SIZE,<uint64>
    Macro,VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE,<uint64>

# Function calls

Remaining lines contain recorded calls to VMA functions. First columns are always:

- Thread ID : uint32
- Time since first call : float, in seconds
- VMA frame index : uint32
- Function name : string

Remaining columns are function parameters and output, depending on function name, which can be:

**vmaCreateAllocator, vmaDestroyAllocator**

No parameters.

**vmaCreatePool**

- memoryTypeIndex : uint32
- flags : uint32
- blockSize : uint64
- minBlockCount : uint64
- maxBlockCount : uint64
- frameInUseCount : uint32
- pool (output) : pointer

**vmaDestroyPool**

- pool : pointer

**vmaSetAllocationUserData**

- allocation : pointer
- pUserData : string (may contain additional commas)

**vmaCreateBuffer**

- bufferCreateInfo.flags : uint32
- bufferCreateInfo.size : uint64
- bufferCreateInfo.usage : uint32
- bufferCreateInfo.sharingMode : uint32
- allocationCreateInfo.flags : uint32
- allocationCreateInfo.usage : uint32
- allocationCreateInfo.requiredFlags : uint32
- allocationCreateInfo.preferredFlags : uint32
- allocationCreateInfo.memoryTypeBits : uint32
- allocationCreateInfo.pool : pointer
- allocation (output) : pointer
- allocationCreateInfo.pUserData : string (may contain additional commas)

**vmaDestroyBuffer**

- allocation : pointer

**vmaCreateImage**

- imageCreateInfo.flags : uint32
- imageCreateInfo.imageType : uint32
- imageCreateInfo.format : uint32
- imageCreateInfo.extent.width : uint32
- imageCreateInfo.extent.height : uint32
- imageCreateInfo.extent.depth : uint32
- imageCreateInfo.mipLevels : uint32
- imageCreateInfo.arrayLayers : uint32
- imageCreateInfo.samples : uint32
- imageCreateInfo.tiling : uint32
- imageCreateInfo.usage : uint32
- imageCreateInfo.sharingMode : uint32
- imageCreateInfo.initialLayout : uint32
- allocationCreateInfo.flags : uint32
- allocationCreateInfo.usage : uint32
- allocationCreateInfo.requiredFlags : uint32
- allocationCreateInfo.preferredFlags : uint32
- allocationCreateInfo.memoryTypeBits : uint32
- allocationCreateInfo.pool : pointer
- allocation (output) : pointer
- allocationCreateInfo.pUserData : string (may contain additional commas)

**vmaDestroyImage**

- allocation : pointer

**vmaFreeMemory** (min format version 1.1)

- allocation : pointer

**vmaCreateLostAllocation** (min format version 1.2)

- allocation (output) : pointer

**vmaAllocateMemory** (min format version 1.2)

- vkMemoryRequirements.size : uint64
- vkMemoryRequirements.alignment : uint64
- vkMemoryRequirements.memoryTypeBits : uint32
- allocationCreateInfo.flags : uint32
- allocationCreateInfo.usage : uint32
- allocationCreateInfo.requiredFlags : uint32
- allocationCreateInfo.preferredFlags : uint32
- allocationCreateInfo.memoryTypeBits : uint32
- allocationCreateInfo.pool : pointer
- allocation (output) : pointer
- allocationCreateInfo.pUserData : string (may contain additional commas)

**vmaAllocateMemoryForBuffer, vmaAllocateMemoryForImage** (min format version 1.2)

- vkMemoryRequirements.size : uint64
- vkMemoryRequirements.alignment : uint64
- vkMemoryRequirements.memoryTypeBits : uint32
- requiresDedicatedAllocation : bool
- prefersDedicatedAllocation : bool
- allocationCreateInfo.flags : uint32
- allocationCreateInfo.usage : uint32
- allocationCreateInfo.requiredFlags : uint32
- allocationCreateInfo.preferredFlags : uint32
- allocationCreateInfo.memoryTypeBits : uint32
- allocationCreateInfo.pool : pointer
- allocation (output) : pointer
- allocationCreateInfo.pUserData : string (may contain additional commas)

**vmaMapMemory, vmaUnmapMemory** (min format version 1.2)

- allocation : pointer

**vmaFlushAllocation, vmaInvalidateAllocation** (min format version 1.2)

- allocation : pointer
- offset : uint64
- size : uint64

**vmaTouchAllocation, vmaGetAllocationInfo** (min format version 1.2)

- allocation : pointer

**vmaMakePoolAllocationsLost** (min format version: 1.2)

- pool : pointer

# Data types

**bool**

Encoded as `0` for false or `1` for true.

**uint32, uint64**

Encoded in decimal format.

**pointer**

Encoded in hexadecimal format.

**pUserData**

If `pUserData` was a pointer, it is encoded as hexadecimal string.
If `VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT` was used with the allocation, the string is written as-is.
It may contain additional commas.
It should not contain end-of-line characters - results are then undefined.

# Example file

    Vulkan Memory Allocator,Calls recording
    1,3
    Config,Begin
    PhysicalDevice,apiVersion,4198477
    PhysicalDevice,driverVersion,8388653
    PhysicalDevice,vendorID,4098
    PhysicalDevice,deviceID,26751
    PhysicalDevice,deviceType,2
    PhysicalDevice,deviceName,Radeon RX Vega
    PhysicalDeviceLimits,maxMemoryAllocationCount,4096
    PhysicalDeviceLimits,bufferImageGranularity,1
    PhysicalDeviceLimits,nonCoherentAtomSize,128
    PhysicalDeviceMemory,HeapCount,3
    PhysicalDeviceMemory,Heap,0,size,8304721920
    PhysicalDeviceMemory,Heap,0,flags,3
    PhysicalDeviceMemory,Heap,1,size,8286175232
    PhysicalDeviceMemory,Heap,1,flags,0
    PhysicalDeviceMemory,Heap,2,size,268435456
    PhysicalDeviceMemory,Heap,2,flags,3
    PhysicalDeviceMemory,TypeCount,4
    PhysicalDeviceMemory,Type,0,heapIndex,0
    PhysicalDeviceMemory,Type,0,propertyFlags,1
    PhysicalDeviceMemory,Type,1,heapIndex,1
    PhysicalDeviceMemory,Type,1,propertyFlags,6
    PhysicalDeviceMemory,Type,2,heapIndex,2
    PhysicalDeviceMemory,Type,2,propertyFlags,7
    PhysicalDeviceMemory,Type,3,heapIndex,1
    PhysicalDeviceMemory,Type,3,propertyFlags,14
    Extension,VK_KHR_dedicated_allocation,1
    Macro,VMA_DEBUG_ALWAYS_DEDICATED_MEMORY,0
    Macro,VMA_DEBUG_ALIGNMENT,1
    Macro,VMA_DEBUG_MARGIN,0
    Macro,VMA_DEBUG_INITIALIZE_ALLOCATIONS,0
    Macro,VMA_DEBUG_DETECT_CORRUPTION,0
    Macro,VMA_DEBUG_GLOBAL_MUTEX,0
    Macro,VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY,1
    Macro,VMA_SMALL_HEAP_MAX_SIZE,1073741824
    Macro,VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE,268435456
    Config,End 
    12552,0.000,0,vmaCreateAllocator
    12552,0.010,0,vmaCreateImage,0,1,37,128,128,1,1,1,1,1,1,0,8,36,2,0,0,0,0000000000000000,000001D85B8B1A80,
    12552,0.010,0,vmaSetAllocationUserData,000001D85B8B1A80,Ala ma kota
    12552,0.015,0,vmaCreateImage,0,1,37,128,128,1,1,1,1,0,6,0,0,0,1,0,0,0,0000000000000000,000001D85B8B1620,
    12552,0.017,0,vmaDestroyImage,000001D85B8B1A80
    12552,0.017,0,vmaCreateBuffer,0,768,1,0,4,2,0,0,0,0000000000000000,000001D85B8B19E0,
    12552,0.017,0,vmaCreateBuffer,0,768,130,0,0,1,0,0,0,0000000000000000,000001D85B8B1A80,
    12552,0.017,0,vmaCreateBuffer,0,60,1,0,4,2,0,0,0,0000000000000000,000001D85B8B1DA0,
    12552,0.017,0,vmaCreateBuffer,0,60,66,0,0,1,0,0,0,0000000000000000,000001D85B8B16C0,
    12552,0.017,0,vmaDestroyBuffer,000001D85B8B1DA0
    12552,0.017,0,vmaDestroyBuffer,000001D85B8B19E0
    12552,0.022,0,vmaCreateImage,0,1,126,1424,704,1,1,1,1,0,32,0,0,0,1,0,0,0,0000000000000000,000001D85B8B1EE0,
    12552,0.048,0,vmaDestroyImage,000001D85B8B1EE0
    12552,0.053,0,vmaCreateImage,0,1,126,1424,704,1,1,1,1,0,32,0,0,0,1,0,0,0,0000000000000000,000001D85B8B1EE0,
    12552,0.662,0,vmaDestroyImage,000001D85B8B1EE0
    12552,0.695,0,vmaDestroyImage,000001D85B8B1620
    12552,0.695,0,vmaDestroyBuffer,000001D85B8B16C0
    12552,0.695,0,vmaDestroyBuffer,000001D85B8B1A80
    12552,0.695,0,vmaDestroyAllocator