# 2.1.0 (2018-09-10)

Minor bugfixes.

# 2.1.0-beta.1 (2018-08-27)

Major release after many months of development in "development" branch and features branches. Many new features added, some bugs fixed. API stays backward-compatible.

Major changes:

- Added linear allocation algorithm, accessible for custom pools, that can be used as free-at-once, stack, double stack, or ring buffer. See "Linear allocation algorithm" documentation chapter.
  - Added `VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT`, `VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT`.
- Added feature to record sequence of calls to the library to a file and replay it using dedicated application. See documentation chapter "Record and replay".
  - Recording: added `VmaAllocatorCreateInfo::pRecordSettings`.
  - Replaying: added VmaReplay project.
  - Recording file format: added document "docs/Recording file format.md".
- Improved support for non-coherent memory.
  - Added functions: `vmaFlushAllocation`, `vmaInvalidateAllocation`.
  - `nonCoherentAtomSize` is now respected automatically.
  - Added `VmaVulkanFunctions::vkFlushMappedMemoryRanges`, `vkInvalidateMappedMemoryRanges`.
- Improved debug features related to detecting incorrect mapped memory usage. See documentation chapter "Debugging incorrect memory usage".
  - Added debug macro `VMA_DEBUG_DETECT_CORRUPTION`, functions `vmaCheckCorruption`, `vmaCheckPoolCorruption`.
  - Added debug macro `VMA_DEBUG_INITIALIZE_ALLOCATIONS` to initialize contents of allocations with a bit pattern.
  - Changed behavior of `VMA_DEBUG_MARGIN` macro - it now adds margin also before first and after last allocation in a block.
- Changed format of JSON dump returned by `vmaBuildStatsString` (not backward compatible!).
  - Custom pools and memory blocks now have IDs that don't change after sorting.
  - Added properties: "CreationFrameIndex", "LastUseFrameIndex", "Usage".
  - Changed VmaDumpVis tool to use these new properties for better coloring.
  - Changed behavior of `vmaGetAllocationInfo` and `vmaTouchAllocation` to update `allocation.lastUseFrameIndex` even if allocation cannot become lost.

Minor changes:

- Changes in custom pools:
  - Added new structure member `VmaPoolStats::blockCount`.
  - Changed behavior of `VmaPoolCreateInfo::blockSize` = 0 (default) - it now means that pool may use variable block sizes, just like default pools do.
- Improved logic of `vmaFindMemoryTypeIndex` for some cases, especially integrated GPUs.
- VulkanSample application: Removed dependency on external library MathFu. Added own vector and matrix structures.
- Changes that improve compatibility with various platforms, including: Visual Studio 2012, 32-bit code, C compilers.
  - Changed usage of "VK_KHR_dedicated_allocation" extension in the code to be optional, driven by macro `VMA_DEDICATED_ALLOCATION`, for compatibility with Android.
- Many additions and fixes in documentation, including description of new features, as well as "Validation layer warnings".
- Other bugfixes.

# 2.0.0 (2018-03-19)

A major release with many compatibility-breaking changes.

Notable new features:

- Introduction of `VmaAllocation` handle that you must retrieve from allocation functions and pass to deallocation functions next to normal `VkBuffer` and `VkImage`.
- Introduction of `VmaAllocationInfo` structure that you can retrieve from `VmaAllocation` handle to access parameters of the allocation (like `VkDeviceMemory` and offset) instead of retrieving them directly from allocation functions.
- Support for reference-counted mapping and persistently mapped allocations - see `vmaMapMemory`, `VMA_ALLOCATION_CREATE_MAPPED_BIT`.
- Support for custom memory pools - see `VmaPool` handle, `VmaPoolCreateInfo` structure, `vmaCreatePool` function.
- Support for defragmentation (compaction) of allocations - see function `vmaDefragment` and related structures.
- Support for "lost allocations" - see appropriate chapter on documentation Main Page.

# 1.0.1 (2017-07-04)

- Fixes for Linux GCC compilation.
- Changed "CONFIGURATION SECTION" to contain #ifndef so you can define these macros before including this header, not necessarily change them in the file.

# 1.0.0 (2017-06-16)

First public release.
