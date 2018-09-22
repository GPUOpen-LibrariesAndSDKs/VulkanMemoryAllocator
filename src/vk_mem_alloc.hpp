#ifndef AMD_VULKAN_MEMORY_ALLOCATOR_HPP
#define AMD_VULKAN_MEMORY_ALLOCATOR_HPP

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include <array>

#if !defined(AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE)
#define AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE vma
#endif

#define AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING VULKAN_HPP_STRINGIFY(AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE)

namespace AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE
{
    using Buffer = VULKAN_HPP_NAMESPACE::Buffer;
    using DeviceSize = VULKAN_HPP_NAMESPACE::DeviceSize;
    using Image = VULKAN_HPP_NAMESPACE::Image;
	using Pool = VmaPool;
	using Allocation = VmaAllocation;
    using Result = VULKAN_HPP_NAMESPACE::Result;

    template<typename T>
    using Optional = VULKAN_HPP_NAMESPACE::Optional<T>;

	template<typename T>
    using ArrayProxy = VULKAN_HPP_NAMESPACE::ArrayProxy<T>;

    template<typename T>
    using ResultValueType = VULKAN_HPP_NAMESPACE::ResultValueType<T>;

    struct DeviceMemoryCallbacks {
        DeviceMemoryCallbacks( PFN_vmaAllocateDeviceMemoryFunction pfnAllocate_ = nullptr,
                                 PFN_vmaFreeDeviceMemoryFunction pfnFree_ = nullptr )
            : pfnAllocate( pfnAllocate_ )
            , pfnFree( pfnFree_ )
        {
        }

        DeviceMemoryCallbacks( VmaDeviceMemoryCallbacks const & rhs )
        {
            memcpy( this, &rhs, sizeof( DeviceMemoryCallbacks ) );
        }

        DeviceMemoryCallbacks& operator=( VmaDeviceMemoryCallbacks const & rhs )
        {
            memcpy( this, &rhs, sizeof( DeviceMemoryCallbacks ) );
            return *this;
        }

        DeviceMemoryCallbacks& setPfnAllocate( PFN_vmaAllocateDeviceMemoryFunction pfnAllocate_ )
        {
            pfnAllocate = pfnAllocate_;
            return *this;
        }

        DeviceMemoryCallbacks& setPfnfree( PFN_vmaFreeDeviceMemoryFunction pfnFree_ )
        {
            pfnFree = pfnFree_;
            return *this;
        }

        operator VmaDeviceMemoryCallbacks const&() const
        {
            return *reinterpret_cast<const VmaDeviceMemoryCallbacks*>(this);
        }

        operator VmaDeviceMemoryCallbacks &()
        {
            return *reinterpret_cast<VmaDeviceMemoryCallbacks*>(this);
        }

        bool operator==( DeviceMemoryCallbacks const& rhs ) const
        {
            return ( pfnAllocate == rhs.pfnAllocate )
                && ( pfnFree == rhs.pfnFree );
        }

        bool operator!=( DeviceMemoryCallbacks const& rhs ) const
        {
            return !operator==( rhs );
        }

        PFN_vmaAllocateDeviceMemoryFunction pfnAllocate;
        PFN_vmaFreeDeviceMemoryFunction pfnFree;
    };
    static_assert( sizeof( DeviceMemoryCallbacks ) == sizeof( VmaDeviceMemoryCallbacks ), "struct and wrapper have different size!" );

    enum class AllocatorCreateFlagBits
    {
        eExternallySynchronized = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
        eKHRDedicatedAllocation = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT
    };

	using AllocatorCreateFlags = VULKAN_HPP_NAMESPACE::Flags<AllocatorCreateFlagBits, VmaAllocatorCreateFlags>;

    struct VulkanFunctions {
        VulkanFunctions( PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties_ = nullptr,
                         PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties_ = nullptr,
                         PFN_vkAllocateMemory vkAllocateMemory_ = nullptr,
                         PFN_vkFreeMemory vkFreeMemory_ = nullptr,
                         PFN_vkMapMemory vkMapMemory_ = nullptr,
                         PFN_vkUnmapMemory vkUnmapMemory_ = nullptr,
                         PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges_ = nullptr,
                         PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges_ = nullptr,
                         PFN_vkBindBufferMemory vkBindBufferMemory_ = nullptr,
                         PFN_vkBindImageMemory vkBindImageMemory_ = nullptr,
                         PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements_ = nullptr,
                         PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements_ = nullptr,
                         PFN_vkCreateBuffer vkCreateBuffer_ = nullptr,
                         PFN_vkDestroyBuffer vkDestroyBuffer_ = nullptr,
                         PFN_vkCreateImage vkCreateImage_ = nullptr,
                         PFN_vkDestroyImage vkDestroyImage_ = nullptr,
#if VMA_DEDICATED_ALLOCATION
                         PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR_ = nullptr,
                         PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR_ = nullptr
#endif
                        )
            : vkGetPhysicalDeviceProperties( vkGetPhysicalDeviceProperties_ )
            , vkGetPhysicalDeviceMemoryProperties( vkGetPhysicalDeviceMemoryProperties_ )
            , vkAllocateMemory( vkAllocateMemory_ )
            , vkFreeMemory( vkFreeMemory_ )
            , vkMapMemory( vkMapMemory_ )
            , vkUnmapMemory( vkUnmapMemory_ )
            , vkFlushMappedMemoryRanges( vkFlushMappedMemoryRanges_ )
            , vkInvalidateMappedMemoryRanges( vkInvalidateMappedMemoryRanges_ )
            , vkBindBufferMemory( vkBindBufferMemory_ )
            , vkBindImageMemory( vkBindImageMemory_ )
            , vkGetBufferMemoryRequirements( vkGetBufferMemoryRequirements_ )
            , vkGetImageMemoryRequirements( vkGetImageMemoryRequirements_ )
            , vkCreateBuffer( vkCreateBuffer_ )
            , vkDestroyBuffer( vkDestroyBuffer_ )
            , vkCreateImage( vkCreateImage_ )
            , vkDestroyImage( vkDestroyImage_ )
#if VMA_DEDICATED_ALLOCATION
            , vkGetBufferMemoryRequirements2KHR( vkGetBufferMemoryRequirements2KHR_ )
            , vkGetImageMemoryRequirements2KHR( vkGetImageMemoryRequirements2KHR_ )
#endif
        {
        }

        VulkanFunctions( VmaVulkanFunctions const & rhs )
        {
            memcpy( this, &rhs, sizeof( VulkanFunctions ) );
        }

        VulkanFunctions& operator=( VmaVulkanFunctions const & rhs )
        {
            memcpy( this, &rhs, sizeof( VulkanFunctions ) );
            return *this;
        }

        VulkanFunctions& setVkGetPhysicalDeviceProperties( PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties_)
        {
            vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties_;
            return *this;
        }

        VulkanFunctions& setVkGetPhysicalDeviceMemoryProperties( PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties_)
        {
            vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties_;
            return *this;
        }

        VulkanFunctions& setVkAllocateMemory( PFN_vkAllocateMemory vkAllocateMemory_)
        {
            vkAllocateMemory = vkAllocateMemory_;
            return *this;
        }

        VulkanFunctions& setVkFreeMemory( PFN_vkFreeMemory vkFreeMemory_)
        {
            vkFreeMemory = vkFreeMemory_;
            return *this;
        }

        VulkanFunctions& setVkMapMemory( PFN_vkMapMemory vkMapMemory_)
        {
            vkMapMemory = vkMapMemory_;
            return *this;
        }

        VulkanFunctions& setVkUnmapMemory( PFN_vkUnmapMemory vkUnmapMemory_)
        {
            vkUnmapMemory = vkUnmapMemory_;
            return *this;
        }

        VulkanFunctions& setVkFlushMappedMemoryRanges( PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges_)
        {
            vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges_;
            return *this;
        }

        VulkanFunctions& setVkInvalidateMappedMemoryRanges( PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges_)
        {
            vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges_;
            return *this;
        }

        VulkanFunctions& setVkBindBufferMemory( PFN_vkBindBufferMemory vkBindBufferMemory_)
        {
            vkBindBufferMemory = vkBindBufferMemory_;
            return *this;
        }

        VulkanFunctions& setVkBindImageMemory( PFN_vkBindImageMemory vkBindImageMemory_)
        {
            vkBindImageMemory = vkBindImageMemory_;
            return *this;
        }

        VulkanFunctions& setVkGetBufferMemoryRequirements( PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements_)
        {
            vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements_;
            return *this;
        }

        VulkanFunctions& setVkGetImageMemoryRequirements( PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements_)
        {
            vkGetImageMemoryRequirements = vkGetImageMemoryRequirements_;
            return *this;
        }

        VulkanFunctions& setVkCreateBuffer( PFN_vkCreateBuffer vkCreateBuffer_)
        {
            vkCreateBuffer = vkCreateBuffer_;
            return *this;
        }

        VulkanFunctions& setVkDestroyBuffer( PFN_vkDestroyBuffer vkDestroyBuffer_)
        {
            vkDestroyBuffer = vkDestroyBuffer_;
            return *this;
        }

        VulkanFunctions& setVkCreateImage( PFN_vkCreateImage vkCreateImage_)
        {
            vkCreateImage = vkCreateImage_;
            return *this;
        }

        VulkanFunctions& setVkDestroyImage( PFN_vkDestroyImage vkDestroyImage_)
        {
            vkDestroyImage = vkDestroyImage_;
            return *this;
        }

#if VMA_DEDICATED_ALLOCATION
        VulkanFunctions& setVkGetBufferMemoryRequirements2KHR( PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR_)
        {
            vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR_;
            return *this;
        }

        VulkanFunctions& setVkGetImageMemoryRequirements2KHR( PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR_)
        {
            vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR_;
            return *this;
        }
#endif

        PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
        PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
        PFN_vkAllocateMemory vkAllocateMemory;
        PFN_vkFreeMemory vkFreeMemory;
        PFN_vkMapMemory vkMapMemory;
        PFN_vkUnmapMemory vkUnmapMemory;
        PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
        PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
        PFN_vkBindBufferMemory vkBindBufferMemory;
        PFN_vkBindImageMemory vkBindImageMemory;
        PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
        PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
        PFN_vkCreateBuffer vkCreateBuffer;
        PFN_vkDestroyBuffer vkDestroyBuffer;
        PFN_vkCreateImage vkCreateImage;
        PFN_vkDestroyImage vkDestroyImage;
#if VMA_DEDICATED_ALLOCATION
        PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
        PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
#endif
    };
    static_assert( sizeof( VulkanFunctions ) == sizeof( VmaVulkanFunctions ), "struct and wrapper have different size!" );

    enum class RecordFlagBits
    {
        eFlushAfterCall = VMA_RECORD_FLUSH_AFTER_CALL_BIT
    };

	using RecordFlags = VULKAN_HPP_NAMESPACE::Flags<RecordFlagBits, VmaRecordFlags>;

    struct RecordSettings
    {
        RecordSettings(RecordFlags flags_ = RecordFlags(),
                        const char* pFilePath_ = nullptr )
            : flags( flags_ )
            , pFilePath( pFilePath_ )
        {
        }

        RecordSettings( VmaRecordSettings const & rhs )
        {
            memcpy( this, &rhs, sizeof( RecordSettings ) );
        }

        RecordSettings& operator=( VmaRecordSettings const & rhs )
        {
            memcpy( this, &rhs, sizeof( RecordSettings ) );
            return *this;
        }

        RecordSettings& setFlags( RecordFlags flags_ )
        {
            flags = flags_;
            return *this;
        }

        RecordSettings& setPFilePath( const char* pFilePath_ )
        {
            pFilePath = pFilePath_;
            return *this;
        }

        operator VmaRecordSettings const&() const
        {
            return *reinterpret_cast<const VmaRecordSettings*>(this);
        }

        operator VmaRecordSettings &()
        {
            return *reinterpret_cast<VmaRecordSettings*>(this);
        }

        bool operator==( RecordSettings const& rhs ) const
        {
            return ( flags == rhs.flags )
                && ( pFilePath == rhs.pFilePath );
        }

        bool operator!=( RecordSettings const& rhs ) const
        {
            return !operator==( rhs );
        }

        RecordFlags flags;
        const char* pFilePath;
    };
    static_assert( sizeof( RecordSettings ) == sizeof( VmaRecordSettings ), "struct and wrapper have different size!" );

    struct AllocatorCreateInfo
    {
        AllocatorCreateInfo( AllocatorCreateFlags flags_ = AllocatorCreateFlags(),
                             VULKAN_HPP_NAMESPACE::PhysicalDevice physicalDevice_ = VULKAN_HPP_NAMESPACE::PhysicalDevice(),
                             VULKAN_HPP_NAMESPACE::Device device_ = VULKAN_HPP_NAMESPACE::Device(),
                             DeviceSize preferredLargeHeapBlockSize_ = 0,
                             const VULKAN_HPP_NAMESPACE::AllocationCallbacks* pAllocationCallbacks_ = nullptr,
                             const DeviceMemoryCallbacks* pDeviceMemoryCallbacks_ = nullptr,
                             uint32_t frameInUseCount_ = 0,
                             const DeviceSize* pHeapSizeLimit_ = nullptr,
                             const VulkanFunctions* pVulkanFunctions_ = nullptr,
                             const RecordSettings* pRecordSettings_ = nullptr )
            : flags(flags_)
            , physicalDevice(physicalDevice_)
            , device(device_)
            , preferredLargeHeapBlockSize(preferredLargeHeapBlockSize_)
            , pAllocationCallbacks(pAllocationCallbacks_)
            , pDeviceMemoryCallbacks(pDeviceMemoryCallbacks_)
            , frameInUseCount(frameInUseCount_)
            , pHeapSizeLimit(pHeapSizeLimit_)
            , pVulkanFunctions(pVulkanFunctions_)
            , pRecordSettings(pRecordSettings_)
        {
        }

        AllocatorCreateInfo( VmaAllocatorCreateInfo const & rhs )
        {
            memcpy( this, &rhs, sizeof( AllocatorCreateInfo ) );
        }

        AllocatorCreateInfo& operator=( VmaAllocatorCreateInfo const & rhs )
        {
            memcpy( this, &rhs, sizeof( AllocatorCreateInfo ) );
            return *this;
        }

        AllocatorCreateInfo& setFlags( AllocatorCreateFlags flags_ )
        {
            flags = flags_;
            return *this;
        }

        AllocatorCreateInfo& setPhysicalDevice( VULKAN_HPP_NAMESPACE::PhysicalDevice physicalDevice_ )
        {
            physicalDevice = physicalDevice_;
            return *this;
        }

        AllocatorCreateInfo& setDevice( VULKAN_HPP_NAMESPACE::Device device_ )
        {
            device = device_;
            return *this;
        }

        AllocatorCreateInfo& setPreferredLargeHeapBlockSize( DeviceSize preferredLargeHeapBlockSize_ )
        {
            preferredLargeHeapBlockSize = preferredLargeHeapBlockSize_;
            return *this;
        }

        AllocatorCreateInfo& setPAllocationCallbacks( const VULKAN_HPP_NAMESPACE::AllocationCallbacks* pAllocationCallbacks_ )
        {
            pAllocationCallbacks = pAllocationCallbacks_;
            return *this;
        }

        AllocatorCreateInfo& setPDeviceMemoryCallbacks( const DeviceMemoryCallbacks* pDeviceMemoryCallbacks_ )
        {
            pDeviceMemoryCallbacks = pDeviceMemoryCallbacks_;
            return *this;
        }

        AllocatorCreateInfo& setFrameInUseCount( uint32_t frameInUseCount_ )
        {
            frameInUseCount = frameInUseCount_;
            return *this;
        }

        AllocatorCreateInfo& setPHeapSizeLimit( const DeviceSize* pHeapSizeLimit_ )
        {
            pHeapSizeLimit = pHeapSizeLimit_;
            return *this;
        }

        AllocatorCreateInfo& setPVulkanFunctions( const VulkanFunctions* pVulkanFunctions_ )
        {
            pVulkanFunctions = pVulkanFunctions_;
            return *this;
        }

        AllocatorCreateInfo& setPRecordSettings( const RecordSettings* pRecordSettings_ )
        {
            pRecordSettings = pRecordSettings_;
            return *this;
        }

        operator VmaAllocatorCreateInfo const&() const
        {
            return *reinterpret_cast<const VmaAllocatorCreateInfo*>(this);
        }

        operator VmaAllocatorCreateInfo &()
        {
            return *reinterpret_cast<VmaAllocatorCreateInfo*>(this);
        }

        bool operator==( AllocatorCreateInfo const& rhs ) const
        {
            return ( flags == rhs.flags )
                && ( physicalDevice == rhs.physicalDevice )
                && ( device == rhs.device )
                && ( preferredLargeHeapBlockSize == rhs.preferredLargeHeapBlockSize )
                && ( pAllocationCallbacks == rhs.pAllocationCallbacks )
                && ( pDeviceMemoryCallbacks == rhs.pDeviceMemoryCallbacks )
                && ( frameInUseCount == rhs.frameInUseCount )
                && ( pHeapSizeLimit == rhs.pHeapSizeLimit )
                && ( pVulkanFunctions == rhs.pVulkanFunctions )
                && ( pRecordSettings == rhs.pRecordSettings );
        }

        bool operator!=( AllocatorCreateInfo const& rhs ) const
        {
            return !operator==( rhs );
        }

        AllocatorCreateFlags flags;
        VULKAN_HPP_NAMESPACE::PhysicalDevice physicalDevice;
        VULKAN_HPP_NAMESPACE::Device device;
        DeviceSize preferredLargeHeapBlockSize;
        const VULKAN_HPP_NAMESPACE::AllocationCallbacks* pAllocationCallbacks;
        const DeviceMemoryCallbacks* pDeviceMemoryCallbacks;
        uint32_t frameInUseCount;
        const DeviceSize* pHeapSizeLimit;
        const VulkanFunctions* pVulkanFunctions;
        const RecordSettings* pRecordSettings;
    };
    static_assert( sizeof( AllocatorCreateInfo ) == sizeof( VmaAllocatorCreateInfo ), "struct and wrapper have different size!" );

    enum class MemoryUsage
    {
        eUnknown = VMA_MEMORY_USAGE_UNKNOWN,
        eGpuOnly = VMA_MEMORY_USAGE_GPU_ONLY,
        eCpuOnly = VMA_MEMORY_USAGE_CPU_ONLY,
        eCpuToGpu = VMA_MEMORY_USAGE_CPU_TO_GPU,
        eGpuToCpu = VMA_MEMORY_USAGE_GPU_TO_CPU
    };

    enum class AllocationCreateFlagBits
    {
        eDedicatedMemory = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        eNeverAllocate = VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT,
        eCanBecomeLost = VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT,
        eCanMakeOtherLost = VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT,
        eUserDataCopyString = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT,
        eUpperAddress = VMA_ALLOCATION_CREATE_UPPER_ADDRESS_BIT
    };

    using AllocationCreateFlags = VULKAN_HPP_NAMESPACE::Flags<AllocationCreateFlagBits, VmaAllocationCreateFlags>;

    enum class PoolCreateFlagBits
    {
        eIgnoreBufferImageGranularity = VMA_POOL_CREATE_IGNORE_BUFFER_IMAGE_GRANULARITY_BIT,
        eLinearAlgorithm = VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT
    };

    using PoolCreateFlags = VULKAN_HPP_NAMESPACE::Flags<PoolCreateFlagBits, VmaPoolCreateFlags>;

    // TODO Add constructors, operators and setters
    struct StatInfo
    {
        uint32_t blockCount = 0;
        uint32_t allocationCount = 0;
        uint32_t unusedRangeCount = 0;
        DeviceSize usedBytes = 0;
        DeviceSize unusedBytes = 0;
        DeviceSize allocationSizeMin = 0, allocationSizeAvg = 0, allocationSizeMax = 0;
        DeviceSize unusedRangeSizeMin = 0, unusedRangeSizeAvg = 0, unusedRangeSizeMax = 0;
    };
    static_assert( sizeof( StatInfo ) == sizeof( VmaStatInfo ), "struct and wrapper have different size!" );

    // TODO Add constructors, operators and setters
    struct Stats
    {
        std::array<StatInfo, VK_MAX_MEMORY_TYPES> memoryType = {0};
        std::array<StatInfo, VK_MAX_MEMORY_HEAPS> memoryHeap = {0};
        StatInfo total;
    };
    static_assert( sizeof( Stats ) == sizeof( VmaStats ), "struct and wrapper have different size!" );

    // TODO Add constructors, operators and setters
    struct AllocationCreateInfo
    {
        AllocationCreateFlags flags = AllocationCreateFlags();
        MemoryUsage usage = MemoryUsage();
        VULKAN_HPP_NAMESPACE::MemoryPropertyFlags requiredFlags = VULKAN_HPP_NAMESPACE::MemoryPropertyFlags();
        VULKAN_HPP_NAMESPACE::MemoryPropertyFlags preferredFlags = VULKAN_HPP_NAMESPACE::MemoryPropertyFlags();
        uint32_t memoryTypeBits = 0;
        Pool pool = VK_NULL_HANDLE;
        void* pUserData = nullptr;
    };
    static_assert( sizeof( AllocationCreateInfo ) == sizeof( VmaAllocationCreateInfo ), "struct and wrapper have different size!" );

    // TODO Add constructors, operators and setters
    struct PoolCreateInfo
    {
        uint32_t memoryTypeIndex = 0;
        PoolCreateFlags flags = PoolCreateFlags();
        DeviceSize blockSize = 0;
        size_t minBlockCount = 0;
        size_t maxBlockCount = 0;
        uint32_t frameInUseCount = 0;
    };
    static_assert( sizeof( PoolCreateInfo ) == sizeof( VmaPoolCreateInfo ), "struct and wrapper have different size!" );

    // TODO Add constructors, operators and setters
    struct PoolStats
    {
        DeviceSize size = 0;
        DeviceSize unusedSize = 0;
        size_t allocationCount = 0;
        size_t unusedRangeCount = 0;
        DeviceSize unusedRangeSizeMax = 0;
        size_t blockCount = 0;
    };
    static_assert( sizeof( PoolStats ) == sizeof( VmaPoolStats ), "struct and wrapper have different size!" );

    // TODO Add constructors, operators and setters
    struct AllocationInfo
    {
        uint32_t memoryType = 0;
        VULKAN_HPP_NAMESPACE::DeviceMemory deviceMemory = VK_NULL_HANDLE;
        DeviceSize offset = 0;
        DeviceSize size = 0;
        void* pMappedData = nullptr;
        void* pUserData = nullptr;
    };
    static_assert( sizeof( AllocationInfo ) == sizeof( VmaAllocationInfo ), "struct and wrapper have different size!" );

    // TODO Add constructors, operators and setters
    struct DefragmentationInfo
    {
        DeviceSize maxBytesToMove = 0;
        uint32_t maxAllocationsToMove = 0;
    };
    static_assert( sizeof( DefragmentationInfo ) == sizeof( VmaDefragmentationInfo ), "struct and wrapper have different size!" );

    // TODO Add constructors, operators and setters
    struct DefragmentationStats
    {
        DeviceSize bytesMoved = 0;
        DeviceSize bytesFreed = 0;
        uint32_t allocationsMoved = 0;
        uint32_t deviceMemoryBlocksFreed = 0;
    };
    static_assert( sizeof( DefragmentationStats ) == sizeof( VmaDefragmentationStats ), "struct and wrapper have different size!" );



    class Allocator
    {
    public:
        VULKAN_HPP_CONSTEXPR Allocator()
            : m_allocator(VK_NULL_HANDLE)
        {}

        VULKAN_HPP_CONSTEXPR Allocator( std::nullptr_t )
            : m_allocator(VK_NULL_HANDLE)
        {}

        VULKAN_HPP_TYPESAFE_EXPLICIT Allocator( VmaAllocator allocator )
            : m_allocator( allocator )
        {}

#if defined(VULKAN_HPP_TYPESAFE_CONVERSION)
        Allocator & operator=(VmaAllocator allocator)
        {
            m_allocator = allocator;
            return *this;
        }
#endif

        Allocator & operator=( std::nullptr_t )
        {
            m_allocator = VK_NULL_HANDLE;
            return *this;
        }

        bool operator==( Allocator const & rhs ) const
        {
            return m_allocator == rhs.m_allocator;
        }

        bool operator!=(Allocator const & rhs ) const
        {
            return m_allocator != rhs.m_allocator;
        }

        bool operator<(Allocator const & rhs ) const
        {
            return m_allocator < rhs.m_allocator;
        }

        void destroy() const { vmaDestroyAllocator( m_allocator ); }

        void getPhysicalDeviceProperties( const VULKAN_HPP_NAMESPACE::PhysicalDeviceProperties** ppPhysicalDeviceProperties )
        {
            vmaGetPhysicalDeviceProperties(
                m_allocator,
                reinterpret_cast<const VkPhysicalDeviceProperties**>( ppPhysicalDeviceProperties ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        const VULKAN_HPP_NAMESPACE::PhysicalDeviceProperties* getPhysicalDeviceProperties()
        {
            const VULKAN_HPP_NAMESPACE::PhysicalDeviceProperties* pPhysicalDeviceProperties;
			getPhysicalDeviceProperties( &pPhysicalDeviceProperties );

            return pPhysicalDeviceProperties;
        }
#endif

        void getMemoryProperties( const VULKAN_HPP_NAMESPACE::PhysicalDeviceMemoryProperties** ppPhysicalDeviceMemoryProperties )
        {
            vmaGetMemoryProperties(
                m_allocator,
                reinterpret_cast<const VkPhysicalDeviceMemoryProperties**>( ppPhysicalDeviceMemoryProperties ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        const VULKAN_HPP_NAMESPACE::PhysicalDeviceMemoryProperties* getMemoryProperties()
        {
            const VULKAN_HPP_NAMESPACE::PhysicalDeviceMemoryProperties* pPhysicalDeviceMemoryProperties;
            getMemoryProperties( &pPhysicalDeviceMemoryProperties );

            return pPhysicalDeviceMemoryProperties;
        }
#endif

        void getMemoryTypeProperties( uint32_t memoryTypeIndex, VULKAN_HPP_NAMESPACE::MemoryPropertyFlags* pFlags )
        {
            vmaGetMemoryTypeProperties(
                m_allocator,
                memoryTypeIndex,
                reinterpret_cast<VkMemoryPropertyFlags*>( pFlags ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
		VULKAN_HPP_NAMESPACE::MemoryPropertyFlags getMemoryTypeProperties( uint32_t memoryTypeIndex )
        {
            VULKAN_HPP_NAMESPACE::MemoryPropertyFlags flags;
            getMemoryTypeProperties( memoryTypeIndex, &flags );

            return flags;
        }
#endif

        void setCurrentFrameIndex( uint32_t frameIndex )
        {
            vmaSetCurrentFrameIndex(
                m_allocator,
                frameIndex );
        }

        void calculateStats( Stats* pStats )
        {
            vmaCalculateStats(
                m_allocator,
                reinterpret_cast<VmaStats*>( &pStats ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        Stats calculateStats()
        {
            Stats stats;
            calculateStats( &stats );

            return stats;
        }
#endif

        void buildStatsString( char** ppStatsString, VkBool32 detailedMap )
        {
            vmaBuildStatsString(
                m_allocator,
                ppStatsString,
                detailedMap );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        char* buildStatsString( VkBool32 detailedMap )
        {
            char* pStatsString;
            buildStatsString(
                &pStatsString,
                detailedMap );

            return pStatsString;
        }
#endif

        void freeStatsString( char* pStatsString )
        {
            vmaFreeStatsString(
                m_allocator,
                pStatsString );
        }

        Result findMemoryTypeIndex( uint32_t memoryTypeBits, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex )
        {
            return static_cast<Result>( vmaFindMemoryTypeIndex(
                m_allocator,
                memoryTypeBits,
                reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ),
                pMemoryTypeIndex ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<uint32_t>::type findMemoryTypeIndex( uint32_t memoryTypeBits, const AllocationCreateInfo& allocationCreateInfo )
        {
            uint32_t memoryTypeIndex;
            Result result = findMemoryTypeIndex( memoryTypeBits, &allocationCreateInfo, &memoryTypeIndex );

            return createResultValue( result, memoryTypeBits, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::findMemoryTypeIndex" );
        }
#endif

        Result findMemoryTypeIndexForBufferInfo( const VULKAN_HPP_NAMESPACE::BufferCreateInfo* pBufferCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex )
        {
            return static_cast<Result>( vmaFindMemoryTypeIndexForBufferInfo(
                m_allocator,
                reinterpret_cast<const VkBufferCreateInfo*>( pBufferCreateInfo ),
                reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ),
                pMemoryTypeIndex ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<uint32_t>::type findMemoryTypeIndexForBufferInfo( const VULKAN_HPP_NAMESPACE::BufferCreateInfo& bufferCreateInfo, const AllocationCreateInfo& allocationCreateInfo )
        {
            uint32_t memoryTypeIndex;
            Result result = findMemoryTypeIndexForBufferInfo( &bufferCreateInfo, &allocationCreateInfo, &memoryTypeIndex );

            return createResultValue( result, memoryTypeIndex, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::findMemoryTypeIndexForBufferInfo" );
        }
#endif

        Result findMemoryTypeIndexForImageInfo( const VULKAN_HPP_NAMESPACE::ImageCreateInfo* pImageCreateInfo, const AllocationCreateInfo* pAllocationCreateInfo, uint32_t* pMemoryTypeIndex )
        {
            return static_cast<Result>( vmaFindMemoryTypeIndexForImageInfo(
                m_allocator,
                reinterpret_cast<const VkImageCreateInfo*>( pImageCreateInfo ),
                reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ),
                pMemoryTypeIndex ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<uint32_t>::type findMemoryTypeIndexForImageInfo( const VULKAN_HPP_NAMESPACE::ImageCreateInfo& imageCreateInfo, const AllocationCreateInfo& allocationCreateInfo )
        {
            uint32_t memoryTypeIndex;
            Result result = findMemoryTypeIndexForImageInfo( &imageCreateInfo, &allocationCreateInfo, &memoryTypeIndex );

            return createResultValue( result, memoryTypeIndex, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::findMemoryTypeIndexForImageInfo" );
        }
#endif

        Result createPool( const PoolCreateInfo* pCreateInfo, Pool* pPool )
        {
            return static_cast<Result>( vmaCreatePool(
                m_allocator,
                reinterpret_cast<const VmaPoolCreateInfo*>( pCreateInfo ),
                reinterpret_cast<VmaPool*>( pPool ) ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<Pool>::type createPool( const PoolCreateInfo& createInfo )
        {
            Pool pool;
            Result result = createPool( &createInfo, &pool );

            return createResultValue( result, pool, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::createPool" );
        }
#endif

        void destroyPool( Pool pool )
        {
            vmaDestroyPool( m_allocator, pool );
        }

        void getPoolStats( Pool pool, PoolStats* pPoolStats )
        {
            vmaGetPoolStats( m_allocator, pool, reinterpret_cast<VmaPoolStats*>( pPoolStats ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        PoolStats getPoolStats( Pool pool )
        {
            PoolStats poolStats;
            getPoolStats( pool, &poolStats );

            return poolStats;
        }
#endif

        void makePoolAllocationsLost( Pool pool, size_t* pLostAllocationCount )
        {
            vmaMakePoolAllocationsLost( m_allocator, pool, pLostAllocationCount );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        size_t makePoolAllocationsLost( Pool pool )
        {
            size_t lostAllocationCount;
            makePoolAllocationsLost( pool, &lostAllocationCount );

            return lostAllocationCount;
        }
#endif

        Result checkPoolCorruption( Pool pool )
        {
            return static_cast<Result>( vmaCheckPoolCorruption( m_allocator, pool) );
        }

        Result allocateMemory( const VULKAN_HPP_NAMESPACE::MemoryRequirements* pVkMemoryRequirements,
            const AllocationCreateInfo* pCreateInfo,
            Allocation* pAllocation,
            AllocationInfo* pAllocationInfo )
        {
            return static_cast<Result>( vmaAllocateMemory( m_allocator,
                reinterpret_cast<const VkMemoryRequirements*>( pVkMemoryRequirements ),
                reinterpret_cast<const VmaAllocationCreateInfo*>( pCreateInfo ),
                reinterpret_cast<VmaAllocation*>( pAllocation ),
                reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<Allocation>::type allocateMemory( const VULKAN_HPP_NAMESPACE::MemoryRequirements& vkMemoryRequirements,
            const AllocationCreateInfo& createInfo,
            Optional<AllocationInfo> pAllocationInfo = nullptr )
        {
            Allocation allocation;
            Result result = allocateMemory( &vkMemoryRequirements, &createInfo, &allocation, pAllocationInfo );

            return createResultValue( result, allocation, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::allocateMemory" );
        }
#endif

        Result allocateMemoryForBuffer( Buffer buffer,
            const AllocationCreateInfo* pCreateInfo,
            Allocation* pAllocation,
            AllocationInfo* pAllocationInfo )
        {
            return static_cast<Result>( vmaAllocateMemoryForBuffer( m_allocator,
                buffer,
                reinterpret_cast<const VmaAllocationCreateInfo*>( pCreateInfo ),
                reinterpret_cast<VmaAllocation*>( pAllocation ),
                reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<Allocation>::type allocateMemoryForBuffer( Buffer buffer,
            const AllocationCreateInfo& createInfo,
            Optional<AllocationInfo> pAllocationInfo = nullptr )
        {
            Allocation allocation;
            Result result = allocateMemoryForBuffer( buffer, &createInfo, &allocation, pAllocationInfo );

            return createResultValue( result, allocation, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::allocateMemoryForBuffer" );
        }
#endif

        Result allocateMemoryForImage( Image image,
            const AllocationCreateInfo* pCreateInfo,
            Allocation* pAllocation,
            AllocationInfo* pAllocationInfo )
        {
            return static_cast<Result>( vmaAllocateMemoryForImage( m_allocator,
                image,
                reinterpret_cast<const VmaAllocationCreateInfo*>( pCreateInfo ),
                reinterpret_cast<VmaAllocation*>( pAllocation ),
                reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ) ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<Allocation>::type allocateMemoryForImage( Image image,
            const AllocationCreateInfo& createInfo,
            Optional<AllocationInfo> pAllocationInfo = nullptr )
        {
            Allocation allocation;
            Result result = allocateMemoryForImage( image, &createInfo, &allocation, pAllocationInfo );

            return createResultValue( result, allocation, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::allocateMemoryForImage" );
        }
#endif

        void freeMemory( Allocation allocation )
        {
            vmaFreeMemory( m_allocator, allocation );
        }

        void getAllocationInfo( Allocation allocation, AllocationInfo* pAllocationInfo )
        {
            vmaGetAllocationInfo( m_allocator,
                allocation,
                reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo ));
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        AllocationInfo getAllocationInfo( Allocation allocation )
        {
            AllocationInfo allocationInfo;
            getAllocationInfo( allocation, &allocationInfo );

            return allocationInfo;
        }
#endif

        VkBool32 touchAllocation( Allocation allocation )
        {
            return vmaTouchAllocation( m_allocator, allocation );
        }

        void setAllocationUserData( Allocation allocation, void* pUserData )
        {
            vmaSetAllocationUserData( m_allocator, allocation, pUserData );
        }

        void createLostAllocation( Allocation* pAllocation )
        {
            vmaCreateLostAllocation( m_allocator, reinterpret_cast<VmaAllocation*>( pAllocation ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        Allocation createLostAllocation()
        {
            Allocation allocation;
            createLostAllocation( &allocation );

            return allocation;
        }
#endif

        Result mapMemory( Allocation allocation, void** ppData )
        {
            return static_cast<Result>( vmaMapMemory( m_allocator, allocation, ppData ) );
        }

        void unmapMemory( Allocation allocation )
        {
            vmaUnmapMemory(m_allocator, allocation );
        }

        void flushMemory( Allocation allocation, DeviceSize offset, DeviceSize size )
        {
            vmaFlushAllocation( m_allocator, allocation, offset, size );
        }

        void invalidateAllocation( Allocation allocation, DeviceSize offset, DeviceSize size )
        {
            vmaInvalidateAllocation( m_allocator, allocation, offset, size );
        }

        Result checkCorruption( uint32_t memoryTypeBits )
        {
            return static_cast<Result>( vmaCheckCorruption( m_allocator, memoryTypeBits ) );
        }

        Result defragment( Allocation* pAllocations,
            size_t allocationCount,
            VkBool32* pAllocationsChanged,
            const DefragmentationInfo* pDefragmentationInfo,
            DefragmentationStats* pDefragmentationStats )
        {
            return static_cast<Result>( vmaDefragment( m_allocator,
                reinterpret_cast<VmaAllocation*>( pAllocations ),
                allocationCount,
                pAllocationsChanged,
                reinterpret_cast<const VmaDefragmentationInfo*>( pDefragmentationInfo ),
                reinterpret_cast<VmaDefragmentationStats*>( pDefragmentationStats ) ) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        Result defragment( ArrayProxy<Allocation> allocations,
            VkBool32* pAllocationsChanged,
            Optional<const DefragmentationInfo> defragmentationInfo,
            Optional<DefragmentationStats> defragmentationStats )
        {
            return defragment( allocations.data(),
                allocations.size(),
                pAllocationsChanged,
                defragmentationInfo,
                defragmentationStats );
        }
#endif

        Result bindBufferMemory( Allocation allocation, Buffer buffer )
        {
            return static_cast<Result>( vmaBindBufferMemory( m_allocator,
                allocation,
                buffer) );
        }

        Result bindImageMemory( Allocation allocation, Image image )
        {
            return static_cast<Result>( vmaBindImageMemory( m_allocator,
                allocation,
                image) );
        }

        Result createBuffer( const VULKAN_HPP_NAMESPACE::BufferCreateInfo* pBufferCreateInfo,
            const AllocationCreateInfo* pAllocationCreateInfo,
            Buffer* pBuffer,
            Allocation* pAllocation,
            AllocationInfo* pAllocationInfo )
        {
            return static_cast<Result>( vmaCreateBuffer( m_allocator,
                reinterpret_cast<const VkBufferCreateInfo*>( pBufferCreateInfo ),
                reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ),
                reinterpret_cast<VkBuffer*>( pBuffer ),
                reinterpret_cast<VmaAllocation*>( pAllocation ),
                reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo )) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<std::pair<Buffer, Allocation>>::type createBuffer( const VULKAN_HPP_NAMESPACE::BufferCreateInfo& bufferCreateInfo,
            const AllocationCreateInfo& allocationCreateInfo,
            AllocationInfo* pAllocationInfo )
        {
            Buffer buffer;
            Allocation allocation;
            Result result = createBuffer( &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, pAllocationInfo );

            return createResultValue( result, std::make_pair( buffer, allocation ), AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::createBuffer" );
        }
#endif

        void destroyBuffer( Buffer buffer, Allocation allocation )
        {
            vmaDestroyBuffer( m_allocator, buffer, allocation );
        }

        Result createImage( const VULKAN_HPP_NAMESPACE::ImageCreateInfo* pImageCreateInfo,
            const AllocationCreateInfo* pAllocationCreateInfo,
            Image* pImage,
            Allocation* pAllocation,
            AllocationInfo* pAllocationInfo )
        {
            return static_cast<Result>( vmaCreateImage( m_allocator,
                reinterpret_cast<const VkImageCreateInfo*>( pImageCreateInfo ),
                reinterpret_cast<const VmaAllocationCreateInfo*>( pAllocationCreateInfo ),
                reinterpret_cast<VkImage*>( pImage ),
                reinterpret_cast<VmaAllocation*>( pAllocation ),
                reinterpret_cast<VmaAllocationInfo*>( pAllocationInfo )) );
        }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
        ResultValueType<std::pair<Image, Allocation>>::type createImage( const VULKAN_HPP_NAMESPACE::ImageCreateInfo& imageCreateInfo,
            const AllocationCreateInfo& allocationCreateInfo,
            AllocationInfo* pAllocationInfo )
        {
            Image image;
            Allocation allocation;
            Result result = createImage( &imageCreateInfo, &allocationCreateInfo, &image, &allocation, pAllocationInfo );

            return createResultValue( result, std::make_pair( image, allocation ), AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::Allocator::createImage" );
        }
#endif

        void destroyImage( Image image, Allocation allocation )
        {
            vmaDestroyImage( m_allocator, image, allocation );
        }

        VULKAN_HPP_TYPESAFE_EXPLICIT operator VmaAllocator() const
        {
            return m_allocator;
        }

        explicit operator bool() const
        {
            return m_allocator != VK_NULL_HANDLE;
        }

        bool operator!() const
        {
            return m_allocator == VK_NULL_HANDLE;
        }

    private:
        VmaAllocator m_allocator;
    };

    template <typename OwnerType>
    class ObjectDestroy
    {
    public:
        ObjectDestroy( OwnerType owner = OwnerType() )
			: m_owner( owner )
        {}

        OwnerType getOwner() const { return m_owner; }

    protected:
        template <typename T>
        void destroy(T t)
        {
			m_owner.destroy( t );
        }

    private:
        OwnerType m_owner;
    };

    class NoParent;

	template <>
    class ObjectDestroy<NoParent>
    {
    public:
        ObjectDestroy() = default;

    protected:
        template <typename T>
        void destroy(T t)
        {
			t.destroy();
        }
    };

#ifndef VULKAN_HPP_NO_SMART_HANDLE

    template <typename Type> class UniqueHandleTraits;

    template <typename Type>
    class UniqueHandle : public UniqueHandleTraits<Type>::deleter
    {
    private:
        using Deleter = typename UniqueHandleTraits<Type>::deleter;
    public:
        explicit UniqueHandle( Type const& value = Type(), Deleter const& deleter = Deleter() )
            : Deleter( deleter)
            , m_value( value )
        {}

        UniqueHandle( UniqueHandle const& ) = delete;

        UniqueHandle( UniqueHandle && other )
            : Deleter( std::move( static_cast<Deleter&>( other ) ) )
            , m_value( other.release() )
        {}

        ~UniqueHandle()
        {
            if ( m_value ) this->destroy( m_value );
        }

        UniqueHandle & operator=( UniqueHandle const& ) = delete;

        UniqueHandle & operator=( UniqueHandle && other )
        {
            reset( other.release() );
            *static_cast<Deleter*>(this) = std::move( static_cast<Deleter&>(other) );
            return *this;
        }

        explicit operator bool() const
        {
            return m_value.operator bool();
        }

        Type const* operator->() const
        {
            return &m_value;
        }

        Type * operator->()
        {
            return &m_value;
        }

        Type const& operator*() const
        {
            return m_value;
        }

        Type & operator*()
        {
            return m_value;
        }

        const Type & get() const
        {
            return m_value;
        }

        Type & get()
        {
            return m_value;
        }

        void reset( Type const& value = Type() )
        {
            if ( m_value != value )
            {
                if ( m_value ) this->destroy( m_value );
                m_value = value;
            }
        }

        Type release()
        {
            Type value = m_value;
            m_value = nullptr;
            return value;
        }

        void swap( UniqueHandle<Type> & rhs )
        {
            std::swap(m_value, rhs.m_value);
            std::swap(static_cast<Deleter&>(*this), static_cast<Deleter&>(rhs));
        }

    private:
        Type    m_value;
    };

    template <typename Type>
    VULKAN_HPP_INLINE void swap( UniqueHandle<Type> & lhs, UniqueHandle<Type> & rhs )
    {
        lhs.swap( rhs );
    }

	template <>
	class UniqueHandleTraits<Allocator> { public: using deleter = ObjectDestroy<NoParent>; };
	using UniqueAllocator = UniqueHandle<Allocator>;
#endif /*VULKAN_HPP_NO_SMART_HANDLE*/

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
	ResultValueType<Allocator>::type createAllocator( const AllocatorCreateInfo & createInfo );
#ifndef VULKAN_HPP_NO_SMART_HANDLE
    typename ResultValueType<UniqueHandle<Allocator>>::type createAllocatorUnique( const AllocatorCreateInfo & createInfo );
#endif /*VULKAN_HPP_NO_SMART_HANDLE*/
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_INLINE Result createAllocator( const AllocatorCreateInfo* pCreateInfo, Allocator* pAllocator )
    {
        return static_cast<Result>( vmaCreateAllocator( reinterpret_cast<const VmaAllocatorCreateInfo*>( pCreateInfo ), reinterpret_cast<VmaAllocator*>( pAllocator ) ) );
    }

#ifndef VULKAN_HPP_DISABLE_ENHANCED_MODE
    VULKAN_HPP_INLINE ResultValueType<Allocator>::type createAllocator( const AllocatorCreateInfo & createInfo )
    {
        Allocator allocator;
		Result result = static_cast<Result>( vmaCreateAllocator( reinterpret_cast<const VmaAllocatorCreateInfo*>( &createInfo ), reinterpret_cast<VmaAllocator*>( &allocator ) ) );
        return VULKAN_HPP_NAMESPACE::createResultValue( result, allocator, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::createAllocator" );
    }

#ifndef VULKAN_HPP_NO_SMART_HANDLE
	template <typename T>
	VULKAN_HPP_INLINE typename ResultValueType<UniqueHandle<T>>::type createResultValue(Result result, T & data, char const * message, typename UniqueHandleTraits<T>::deleter const& deleter)
	{
		VULKAN_HPP_ASSERT(result == VULKAN_HPP_NAMESPACE::Result::eSuccess);
		return ResultValue<UniqueHandle<T>>(result, UniqueHandle<T>(data, deleter));

		if (result != VULKAN_HPP_NAMESPACE::Result::eSuccess)
		{
			VULKAN_HPP_NAMESPACE::throwResultException(result, message);
		}
		return UniqueHandle<T>(data, deleter);
	}
#endif//*/

#ifndef VULKAN_HPP_NO_SMART_HANDLE
    VULKAN_HPP_INLINE typename ResultValueType<UniqueHandle<Allocator>>::type createAllocatorUnique( const AllocatorCreateInfo & createInfo )
	{
        Allocator allocator;
		VULKAN_HPP_NAMESPACE::Result result = static_cast<VULKAN_HPP_NAMESPACE::Result>( vmaCreateAllocator( reinterpret_cast<const VmaAllocatorCreateInfo*>( &createInfo ), reinterpret_cast<VmaAllocator*>( &allocator ) ) );

#ifdef VULKAN_HPP_NO_EXCEPTIONS
		VULKAN_HPP_ASSERT(result == VULKAN_HPP_NAMESPACE::Result::eSuccess);
		return VULKAN_HPP_NAMESPACE::ResultValue<UniqueHandle<Allocator>>(result, UniqueHandle<Allocator>(allocator)).value;
#else
		if (result != VULKAN_HPP_NAMESPACE::Result::eSuccess)
		{
			throwResultException(result, AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE_STRING"::createAllocatorUnique");
		}
		return UniqueHandle<Allocator>(allocator);
#endif
    }
#endif /*VULKAN_HPP_NO_SMART_HANDLE*/
#endif /*VULKAN_HPP_DISABLE_ENHANCED_MODE*/

    VULKAN_HPP_INLINE std::string to_string(AllocatorCreateFlagBits value)
    {
        switch (value)
        {
        case AllocatorCreateFlagBits::eExternallySynchronized: return "ExternallySynchronized";
        case AllocatorCreateFlagBits::eKHRDedicatedAllocation: return "KHRDedicatedAllocation";
        default: return "invalid";
        }
    }

    VULKAN_HPP_INLINE std::string to_string(AllocatorCreateFlags value)
    {
        if (!value) return "{}";
        std::string result;
        if (value & AllocatorCreateFlagBits::eExternallySynchronized) result += "ExternallySynchronized | ";
        if (value & AllocatorCreateFlagBits::eKHRDedicatedAllocation) result += "KHRDedicatedAllocation | ";
        return "{" + result.substr(0, result.size() - 3) + "}";
    }

    VULKAN_HPP_INLINE std::string to_string(RecordFlagBits value)
    {
        switch (value)
        {
        case RecordFlagBits::eFlushAfterCall: return "FlushAfterCall";
        default: return "invalid";
        }
    }

    VULKAN_HPP_INLINE std::string to_string(RecordFlags value)
    {
        if (!value) return "{}";
        std::string result;
        if (value & RecordFlagBits::eFlushAfterCall) result += "FlushAfterCall | ";
        return "{" + result.substr(0, result.size() - 3) + "}";
    }

    VULKAN_HPP_INLINE std::string to_string(MemoryUsage value)
    {
        switch (value)
        {
        case MemoryUsage::eUnknown: return "Unknown";
        case MemoryUsage::eGpuOnly: return "GpuOnly";
        case MemoryUsage::eCpuOnly: return "CpuOnly";
        case MemoryUsage::eCpuToGpu: return "CpuToGpu";
        case MemoryUsage::eGpuToCpu: return "GpuToCpu";
        default: return "invalid";
        }
    }

    VULKAN_HPP_INLINE std::string to_string(AllocationCreateFlagBits value)
    {
        switch (value)
        {
        case AllocationCreateFlagBits::eDedicatedMemory: return "DedicatedMemory";
        case AllocationCreateFlagBits::eNeverAllocate: return "NeverAllocate";
        case AllocationCreateFlagBits::eCanBecomeLost: return "CanBecomeLost";
        case AllocationCreateFlagBits::eCanMakeOtherLost: return "CanMakeOtherLost";
        case AllocationCreateFlagBits::eUserDataCopyString: return "UserDataCopyString";
        case AllocationCreateFlagBits::eUpperAddress: return "UpperAddress";
        default: return "invalid";
        }
    }

    VULKAN_HPP_INLINE std::string to_string(AllocationCreateFlags value)
    {
        if (!value) return "{}";
        std::string result;
        if (value & AllocationCreateFlagBits::eDedicatedMemory) result += "DedicatedMemory | ";
        if (value & AllocationCreateFlagBits::eNeverAllocate) result += "NeverAllocate | ";
        if (value & AllocationCreateFlagBits::eCanBecomeLost) result += "CanBecomeLost | ";
        if (value & AllocationCreateFlagBits::eCanMakeOtherLost) result += "CanMakeOtherLost | ";
        if (value & AllocationCreateFlagBits::eUserDataCopyString) result += "UserDataCopyString | ";
        if (value & AllocationCreateFlagBits::eUpperAddress) result += "UpperAddress | ";
        return "{" + result.substr(0, result.size() - 3) + "}";
    }

    VULKAN_HPP_INLINE std::string to_string(PoolCreateFlagBits value)
    {
        switch (value)
        {
        case PoolCreateFlagBits::eIgnoreBufferImageGranularity: return "IgnoreBufferImageGranularity";
        case PoolCreateFlagBits::eLinearAlgorithm: return "LinearAlgorithm";
        default: return "invalid";
        }
    }

    VULKAN_HPP_INLINE std::string to_string(PoolCreateFlags value)
    {
        if (!value) return "{}";
        std::string result;
        if (value & PoolCreateFlagBits::eIgnoreBufferImageGranularity) result += "IgnoreBufferImageGranularity | ";
        if (value & PoolCreateFlagBits::eLinearAlgorithm) result += "LinearAlgorithm | ";
        return "{" + result.substr(0, result.size() - 3) + "}";
    }
} // namespace AMD_VULKAN_MEMORY_ALLOCATOR_HPP_NAMESPACE

#endif // AMD_VULKAN_MEMORY_ALLOCATOR_H
