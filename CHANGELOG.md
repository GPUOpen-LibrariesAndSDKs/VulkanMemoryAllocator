# 2.0.0-alpha (2017-09-12)

A major release with many compatibility-breaking changes.

This code is work in progress and subject to changes.

Notable new features:

- Introduction of `VmaAllocation` handle that you must retrieve from allocation functions and pass to deallocation functions next to normal `VkBuffer` and `VkImage`.
- Introduction of `VmaAllocationInfo` structure that you can retrieve from `VmaAllocation` handle to access parameters of the allocation (like `VkDeviceMemory` and offset) instead of retrieving them directly from allocation functions.
- Support for persistently mapped allocations - see `VMA_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT`.
- Support for custom memory pools - `VmaPool` handle, `VmaPoolCreateInfo` structure, `vmaCreatePool` function.
- Support for defragmentation (compaction) of allocations - see function `vmaDefragment` and related structures.
- Support for "lost allocations" - see appropriate chapter on documentation Main Page.

# 1.0.1 (2017-07-04)

- Fixes for Linux GCC compilation.
- Changed "CONFIGURATION SECTION" to contain #ifndef so you can define these macros before including this header, not necessarily change them in the file.

# 1.0.0 (2017-06-16)

First public release.
