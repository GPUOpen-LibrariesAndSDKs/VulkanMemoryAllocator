#include "Common.h"
#include "SparseBindingTest.h"

#ifdef _WIN32

////////////////////////////////////////////////////////////////////////////////
// External imports

extern VkDevice g_hDevice;
extern VmaAllocator g_hAllocator;
extern uint32_t g_FrameIndex;
extern bool g_SparseBindingEnabled;
extern VkQueue g_hSparseBindingQueue;
extern VkFence g_ImmediateFence;

void SaveAllocatorStatsToFile(const wchar_t* filePath);

////////////////////////////////////////////////////////////////////////////////
// Class definitions

class BaseImage
{
public:
    virtual void Init(RandomNumberGenerator& rand) = 0;
    virtual ~BaseImage();

protected:
    VkImage m_Image = VK_NULL_HANDLE;

    void FillImageCreateInfo(VkImageCreateInfo& outInfo, RandomNumberGenerator& rand);
};

class TraditionalImage : public BaseImage
{
public:
    virtual void Init(RandomNumberGenerator& rand);
    virtual ~TraditionalImage();

private:
    VmaAllocation m_Allocation = VK_NULL_HANDLE;
};

class SparseBindingImage : public BaseImage
{
public:
    virtual void Init(RandomNumberGenerator& rand);
    virtual ~SparseBindingImage();

private:
    std::vector<VmaAllocation> m_Allocations;
};

////////////////////////////////////////////////////////////////////////////////
// class BaseImage

BaseImage::~BaseImage()
{
    if(m_Image)
    {
        vkDestroyImage(g_hDevice, m_Image, nullptr);
    }
}

void BaseImage::FillImageCreateInfo(VkImageCreateInfo& outInfo, RandomNumberGenerator& rand)
{
    constexpr uint32_t imageSizeMin = 8;
    constexpr uint32_t imageSizeMax = 2048;

    ZeroMemory(&outInfo, sizeof(outInfo));
    outInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    outInfo.imageType = VK_IMAGE_TYPE_2D;
    outInfo.extent.width = rand.Generate() % (imageSizeMax - imageSizeMin) + imageSizeMin;
    outInfo.extent.height = rand.Generate() % (imageSizeMax - imageSizeMin) + imageSizeMin;
    outInfo.extent.depth = 1;
    outInfo.mipLevels = 1; // TODO ?
    outInfo.arrayLayers = 1;
    outInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    outInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    outInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    outInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    outInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    outInfo.flags = 0;
}

////////////////////////////////////////////////////////////////////////////////
// class TraditionalImage

void TraditionalImage::Init(RandomNumberGenerator& rand)
{
    VkImageCreateInfo imageCreateInfo;
    FillImageCreateInfo(imageCreateInfo, rand);

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    // Default BEST_FIT is clearly better.
    //allocCreateInfo.flags |= VMA_ALLOCATION_CREATE_STRATEGY_WORST_FIT_BIT;
    
    ERR_GUARD_VULKAN( vmaCreateImage(g_hAllocator, &imageCreateInfo, &allocCreateInfo,
        &m_Image, &m_Allocation, nullptr) );
}

TraditionalImage::~TraditionalImage()
{
    if(m_Allocation)
    {
        vmaFreeMemory(g_hAllocator, m_Allocation);
    }
}

////////////////////////////////////////////////////////////////////////////////
// class SparseBindingImage

void SparseBindingImage::Init(RandomNumberGenerator& rand)
{
    assert(g_SparseBindingEnabled && g_hSparseBindingQueue);

    // Create image.
    VkImageCreateInfo imageCreateInfo;
    FillImageCreateInfo(imageCreateInfo, rand);
    imageCreateInfo.flags |= VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
    ERR_GUARD_VULKAN( vkCreateImage(g_hDevice, &imageCreateInfo, nullptr, &m_Image) );

    // Get memory requirements.
    VkMemoryRequirements imageMemReq;
    vkGetImageMemoryRequirements(g_hDevice, m_Image, &imageMemReq);

    // This is just to silence validation layer warning.
    // But it doesn't help. Looks like a bug in Vulkan validation layers.
    uint32_t sparseMemReqCount = 0;
    vkGetImageSparseMemoryRequirements(g_hDevice, m_Image, &sparseMemReqCount, nullptr);
    assert(sparseMemReqCount <= 8);
    VkSparseImageMemoryRequirements sparseMemReq[8];
    vkGetImageSparseMemoryRequirements(g_hDevice, m_Image, &sparseMemReqCount, sparseMemReq);

    // According to Vulkan specification, for sparse resources memReq.alignment is also page size.
    const VkDeviceSize pageSize = imageMemReq.alignment;
    const uint32_t pageCount = (uint32_t)ceil_div<VkDeviceSize>(imageMemReq.size, pageSize);

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkMemoryRequirements pageMemReq = imageMemReq;
    pageMemReq.size = pageSize;

    // Allocate and bind memory pages.
    m_Allocations.resize(pageCount);
    std::fill(m_Allocations.begin(), m_Allocations.end(), nullptr);
    std::vector<VkSparseMemoryBind> binds{pageCount};
    std::vector<VmaAllocationInfo> allocInfo{pageCount};
    ERR_GUARD_VULKAN( vmaAllocateMemoryPages(g_hAllocator, &pageMemReq, &allocCreateInfo, pageCount, m_Allocations.data(), allocInfo.data()) );

    for(uint32_t i = 0; i < pageCount; ++i)
    {
        binds[i] = {};
        binds[i].resourceOffset = pageSize * i;
        binds[i].size = pageSize;
        binds[i].memory = allocInfo[i].deviceMemory;
        binds[i].memoryOffset = allocInfo[i].offset;
    }

    VkSparseImageOpaqueMemoryBindInfo imageBindInfo;
    imageBindInfo.image = m_Image;
    imageBindInfo.bindCount = pageCount;
    imageBindInfo.pBinds = binds.data();

    VkBindSparseInfo bindSparseInfo = { VK_STRUCTURE_TYPE_BIND_SPARSE_INFO };
    bindSparseInfo.pImageOpaqueBinds = &imageBindInfo;
    bindSparseInfo.imageOpaqueBindCount = 1;
    
    ERR_GUARD_VULKAN( vkResetFences(g_hDevice, 1, &g_ImmediateFence) );
    ERR_GUARD_VULKAN( vkQueueBindSparse(g_hSparseBindingQueue, 1, &bindSparseInfo, g_ImmediateFence) );
    ERR_GUARD_VULKAN( vkWaitForFences(g_hDevice, 1, &g_ImmediateFence, VK_TRUE, UINT64_MAX) );
}

SparseBindingImage::~SparseBindingImage()
{
    vmaFreeMemoryPages(g_hAllocator, m_Allocations.size(), m_Allocations.data());
}

////////////////////////////////////////////////////////////////////////////////
// Private functions

////////////////////////////////////////////////////////////////////////////////
// Public functions

void TestSparseBinding()
{
    struct ImageInfo
    {
        std::unique_ptr<BaseImage> image;
        uint32_t endFrame;
    };
    std::vector<ImageInfo> images;

    constexpr uint32_t frameCount = 2000;
    constexpr uint32_t imageLifeFramesMin = 1;
    constexpr uint32_t imageLifeFramesMax = 400;

    RandomNumberGenerator rand(4652467);

    for(uint32_t i = 0; i < frameCount; ++i)
    {
        // Bump frame index.
        ++g_FrameIndex;
        vmaSetCurrentFrameIndex(g_hAllocator, g_FrameIndex);

        // Create one new, random image.
        ImageInfo imageInfo;
        //imageInfo.image = std::make_unique<TraditionalImage>();
        imageInfo.image = std::make_unique<SparseBindingImage>();
        imageInfo.image->Init(rand);
        imageInfo.endFrame = g_FrameIndex + rand.Generate() % (imageLifeFramesMax - imageLifeFramesMin) + imageLifeFramesMin;
        images.push_back(std::move(imageInfo));

        // Delete all images that expired.
        for(size_t i = images.size(); i--; )
        {
            if(g_FrameIndex >= images[i].endFrame)
            {
                images.erase(images.begin() + i);
            }
        }
    }

    SaveAllocatorStatsToFile(L"SparseBindingTest.json");

    // Free remaining images.
    images.clear();
}

#endif // #ifdef _WIN32
