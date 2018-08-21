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

    1,2

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

## Data types

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
    1,2
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