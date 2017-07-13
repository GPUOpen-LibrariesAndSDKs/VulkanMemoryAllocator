# 2.0.0-alpha (2017-07-13)

This code is work in progress and subject to changes.

Notable new features:

- Support for persistently mapped allocations - see VMA_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT.
- Introduction of VmaAllocation handle that you must retrieve from allocation functions and pass to deallocation functions, possibly next to normal VkBuffer and VkImage.
- Introduction of VmaAllocationInfo structure that you can retrieve from VmaAllocation handle to access parameters of the allocation (like VkDeviceMemory and offset) instead of retrieving them directly from allocation functions.
- Support for defragmentation (compaction) of allocations - see function vmaDefragment and related structures.

Compatibility-breaking changes:

- Modified struct VmaMemoryRequirements: removed member neverAllocate, added members: flags, pUserData. Added typedef VmaMemoryRequirementFlags, enum VmaMemoryRequirementFlagBits. 
- Added VmaAllocation handle and VmaAllocationInfo structure. They are now used in modified signatures of functions: vmaAllocateMemory, vmaAllocateMemoryForBuffer, vmaAllocateMemoryForImage, vmaFreeMemory, vmaMapMemory, vmaUnmapMemory, vmaCreateBuffer, vmaDestroyBuffer, vmaCreateImage, vmaDestroyImage.

Additions:

- Added new functions related to introduction of VmaAllocation: vmaGetAllocationInfo, vmaSetAllocationUserData.
- Added support for temporarily unmapping persistently mapped memory for performance reasons: vmaUnmapPersistentlyMappedMemory, vmaMapPersistentlyMappedMemory.
- Added support for defragmentation: function vmaDefragment, structures: VmaDefragmentationInfo, VmaDefragmentationStats.
- Added struct member VmaAllocatorCreateInfo::pDeviceMemoryCallbacks, struct VmaDeviceMemoryCallbacks, function pointers: PFN_vmaAllocateDeviceMemoryFunction, PFN_vmaFreeDeviceMemoryFunction. They can be used to notify the user about calls to vkAllocateMemory and vkFreeMemory made by the library.
- Added struct member VmaAllocatorCreateInfo::flags, typedef VmaAllocatorFlags, enum VmaAllocatorFlagBits, enum value VMA_ALLOCATOR_EXTERNALLY_SYNCHRONIZED_BIT. It can be used to disable internal synchronization of an allocator for better performance if user wants to use it from single thread only or synchronize it externally.

# 1.0.1 (2017-07-04)

- Fixes for Linux GCC compilation.
- Changed "CONFIGURATION SECTION" to contain #ifndef so you can define these macros before including this header, not necessarily change them in the file.

# 1.0.0 (2017-06-16)

First public release.
