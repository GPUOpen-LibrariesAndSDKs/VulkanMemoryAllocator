//
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
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

#include "VmaUsage.h"
#include "Common.h"
#include <unordered_map>

static const int RESULT_EXCEPTION          = -1000;
static const int RESULT_ERROR_COMMAND_LINE = -1;
static const int RESULT_ERROR_SOURCE_FILE  = -2;
static const int RESULT_ERROR_FORMAT       = -3;

struct StrRange
{
    const char* beg;
    const char* end;

    size_t length() const { return end - beg; }
};

static inline bool StrRangeEq(const StrRange& lhs, const char* rhsSz)
{
    const size_t rhsLen = strlen(rhsSz);
    return rhsLen == lhs.length() &&
        memcmp(lhs.beg, rhsSz, rhsLen) == 0;
}

static inline bool StrRangeToUint(const StrRange& s, uint32_t& out)
{
    // TODO handle failure.
    char* end = (char*)s.end;
    out = (uint32_t)strtoul(s.beg, &end, 10);
    return true;
}
static inline bool StrRangeToUint(const StrRange& s, uint64_t& out)
{
    // TODO handle failure.
    char* end = (char*)s.end;
    out = (uint64_t)strtoull(s.beg, &end, 10);
    return true;
}
static inline bool StrRangeToPtr(const StrRange& s, uint64_t& out)
{
    // TODO handle failure.
    char* end = (char*)s.end;
    out = (uint64_t)strtoull(s.beg, &end, 16);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// LineSplit class

class LineSplit
{
public:
    LineSplit(const char* data, size_t numBytes) :
        m_Data(data),
        m_NumBytes(numBytes),
        m_NextLineBeg(0),
        m_NextLineIndex(0)
    {
    }

    bool GetNextLine(StrRange& out);
    size_t GetNextLineIndex() const { return m_NextLineIndex; }

private:
    const char* const m_Data;
    const size_t m_NumBytes;
    size_t m_NextLineBeg;
    size_t m_NextLineIndex;
};

bool LineSplit::GetNextLine(StrRange& out)
{
    if(m_NextLineBeg < m_NumBytes)
    {
        out.beg = m_Data + m_NextLineBeg;
        size_t currLineEnd = m_NextLineBeg;
        while(currLineEnd < m_NumBytes && m_Data[currLineEnd] != '\n')
            ++currLineEnd;
        out.end = m_Data + currLineEnd;
        m_NextLineBeg = currLineEnd + 1; // Past '\n'
        ++m_NextLineIndex;
        return true;
    }
    else
        return false;
}


////////////////////////////////////////////////////////////////////////////////
// CsvSplit class

class CsvSplit
{
public:
    static const size_t RANGE_COUNT_MAX = 32;

    void Set(const StrRange& line, size_t maxCount = RANGE_COUNT_MAX);

    size_t GetCount() const { return m_Count; }
    StrRange GetRange(size_t index) const 
    {
        return StrRange {
            m_Str + m_Ranges[index * 2],
            m_Str + m_Ranges[index * 2 + 1] };
    }

private:
    const char* m_Str = nullptr;
    size_t m_Count = 0;
    size_t m_Ranges[RANGE_COUNT_MAX * 2]; // Pairs of begin-end.
};

void CsvSplit::Set(const StrRange& line, size_t maxCount)
{
    assert(maxCount <= RANGE_COUNT_MAX);
    m_Str = line.beg;
    const size_t strLen = line.length();
    size_t rangeIndex = 0;
    size_t charIndex = 0;
    while(charIndex < strLen && rangeIndex < maxCount)
    {
        m_Ranges[rangeIndex * 2] = charIndex;
        while(charIndex < strLen && (rangeIndex + 1 == maxCount || m_Str[charIndex] != ','))
            ++charIndex;
        m_Ranges[rangeIndex * 2 + 1] = charIndex;
        ++rangeIndex;
        ++charIndex; // Past ','
    }
    m_Count = rangeIndex;
}

////////////////////////////////////////////////////////////////////////////////
// class Player

static const char* const VALIDATION_LAYER_NAME = "VK_LAYER_LUNARG_standard_validation";

static bool g_MemoryAliasingWarningEnabled = true;
static bool g_EnableValidationLayer = true;
static bool VK_KHR_get_memory_requirements2_enabled = false;
static bool VK_KHR_dedicated_allocation_enabled = false;

static VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
{
    // "Non-linear image 0xebc91 is aliased with linear buffer 0xeb8e4 which may indicate a bug."
    if(!g_MemoryAliasingWarningEnabled && flags == VK_DEBUG_REPORT_WARNING_BIT_EXT &&
        (strstr(pMessage, " is aliased with non-linear ") || strstr(pMessage, " is aliased with linear ")))
    {
        return VK_FALSE;
    }

    // Ignoring because when VK_KHR_dedicated_allocation extension is enabled,
    // vkGetBufferMemoryRequirements2KHR function is used instead, while Validation
    // Layer seems to be unaware of it.
    if (strstr(pMessage, "but vkGetBufferMemoryRequirements() has not been called on that buffer") != nullptr)
    {
        return VK_FALSE;
    }
    if (strstr(pMessage, "but vkGetImageMemoryRequirements() has not been called on that image") != nullptr)
    {
        return VK_FALSE;
    }
    
    printf("%s \xBA %s\n", pLayerPrefix, pMessage);

    return VK_FALSE;
}

static bool IsLayerSupported(const VkLayerProperties* pProps, size_t propCount, const char* pLayerName)
{
    const VkLayerProperties* propsEnd = pProps + propCount;
    return std::find_if(
        pProps,
        propsEnd,
        [pLayerName](const VkLayerProperties& prop) -> bool {
            return strcmp(pLayerName, prop.layerName) == 0;
        }) != propsEnd;
}

static const size_t FIRST_PARAM_INDEX = 4;

class Player
{
public:
    Player();
    void Init();
    ~Player();

    void ExecuteLine(size_t lineNumber, const StrRange& line);

private:
    static const size_t MAX_WARNINGS_TO_SHOW = 64;

    size_t m_WarningCount = 0;

    VkInstance m_VulkanInstance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    uint32_t m_GraphicsQueueFamilyIndex = UINT_MAX;
    VkDevice m_Device = VK_NULL_HANDLE;
    VmaAllocator m_Allocator = VK_NULL_HANDLE;

    PFN_vkCreateDebugReportCallbackEXT m_pvkCreateDebugReportCallbackEXT;
    PFN_vkDebugReportMessageEXT m_pvkDebugReportMessageEXT;
    PFN_vkDestroyDebugReportCallbackEXT m_pvkDestroyDebugReportCallbackEXT;
    VkDebugReportCallbackEXT m_hCallback;

    uint32_t m_VmaFrameIndex = 0;

    // Any of these handles null means it was created in original but couldn't be created now.
    struct Pool
    {
        VmaPool pool;
    };
    struct Buffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
    };
    struct Image
    {
        VkImage image;
        VmaAllocation allocation;
    };

    std::unordered_map<uint64_t, Pool> m_Pools;
    std::unordered_map<uint64_t, Buffer> m_Buffers;
    std::unordered_map<uint64_t, Image> m_Images;

    // Increments warning counter. Returns true if warning message should be printed.
    bool IssueWarning() { return m_WarningCount++ < MAX_WARNINGS_TO_SHOW; }

    void InitVulkan();
    void FinalizeVulkan();
    void RegisterDebugCallbacks();

    // If parmeter count doesn't match, issues warning and returns false.
    bool ValidateFunctionParameterCount(size_t lineNumber, const CsvSplit& csvSplit, size_t expectedParamCount, bool lastUnbound);

    void CreatePool(size_t lineNumber, const CsvSplit& csvSplit);
    void DestroyPool(size_t lineNumber, const CsvSplit& csvSplit);
    void CreateBuffer(size_t lineNumber, const CsvSplit& csvSplit);
    void DestroyBuffer(size_t lineNumber, const CsvSplit& csvSplit);
    void CreateImage(size_t lineNumber, const CsvSplit& csvSplit);
    void DestroyImage(size_t lineNumber, const CsvSplit& csvSplit);
};

Player::Player()
{
}

void Player::Init()
{
    InitVulkan();
}

Player::~Player()
{
    FinalizeVulkan();

    if(m_WarningCount > MAX_WARNINGS_TO_SHOW)
        printf("WARNING: %zu more warnings not shown.\n", m_WarningCount - MAX_WARNINGS_TO_SHOW);
}

void Player::ExecuteLine(size_t lineNumber, const StrRange& line)
{
    CsvSplit csvSplit;
    csvSplit.Set(line);

    if(csvSplit.GetCount() >= FIRST_PARAM_INDEX)
    {
        // Update VMA current frame index.
        StrRange frameIndexStr = csvSplit.GetRange(2);
        uint32_t frameIndex;
        if(StrRangeToUint(frameIndexStr, frameIndex))
        {
            if(frameIndex != m_VmaFrameIndex)
            {
                vmaSetCurrentFrameIndex(m_Allocator, frameIndex);
                m_VmaFrameIndex = frameIndex;
            }
        }
        else
        {
            if(IssueWarning())
                printf("Line %zu: Incorrect frame index.\n", lineNumber);
        }

        StrRange functionName = csvSplit.GetRange(3);

        if(StrRangeEq(functionName, "vmaCreateAllocator"))
        {
            if(ValidateFunctionParameterCount(lineNumber, csvSplit, 0, false))
            {
                // Nothing to do.
            }
        }
        else if(StrRangeEq(functionName, "vmaCreatePool"))
            CreatePool(lineNumber, csvSplit);
        else if(StrRangeEq(functionName, "vmaDestroyPool"))
            DestroyPool(lineNumber, csvSplit);
        else if(StrRangeEq(functionName, "vmaSetAllocationUserData"))
        {
            if(ValidateFunctionParameterCount(lineNumber, csvSplit, 2, true))
            {
                // Ignore for now.
            }
        }
        else if(StrRangeEq(functionName, "vmaCreateBuffer"))
            CreateBuffer(lineNumber, csvSplit);
        else if(StrRangeEq(functionName, "vmaDestroyBuffer"))
            DestroyBuffer(lineNumber, csvSplit);
        else if(StrRangeEq(functionName, "vmaCreateImage"))
            CreateImage(lineNumber, csvSplit);
        else if(StrRangeEq(functionName, "vmaDestroyImage"))
            DestroyImage(lineNumber, csvSplit);
        else
        {
            if(IssueWarning())
                printf("Line %zu: Unknown function.\n", lineNumber);
        }
    }
    else
    {
        if(IssueWarning())
            printf("Line %zu: Too few columns.\n", lineNumber);
    }
}

void Player::InitVulkan()
{
    printf("Initializing Vulkan...\n");

    uint32_t instanceLayerPropCount = 0;
    ERR_GUARD_VULKAN( vkEnumerateInstanceLayerProperties(&instanceLayerPropCount, nullptr) );
    std::vector<VkLayerProperties> instanceLayerProps(instanceLayerPropCount);
    if(instanceLayerPropCount > 0)
    {
        ERR_GUARD_VULKAN( vkEnumerateInstanceLayerProperties(&instanceLayerPropCount, instanceLayerProps.data()) );
    }

    if(g_EnableValidationLayer == true)
    {
        if(IsLayerSupported(instanceLayerProps.data(), instanceLayerProps.size(), VALIDATION_LAYER_NAME) == false)
        {
            printf("WARNING: Layer \"%s\" not supported.\n", VALIDATION_LAYER_NAME);
            g_EnableValidationLayer = false;
        }
    }

    std::vector<const char*> instanceExtensions;
    //instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    //instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    std::vector<const char*> instanceLayers;
    if(g_EnableValidationLayer == true)
    {
        instanceLayers.push_back(VALIDATION_LAYER_NAME);
        instanceExtensions.push_back("VK_EXT_debug_report");
    }

    VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName = "VmaReplay";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Vulkan Memory Allocator";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instInfo.pApplicationInfo = &appInfo;
    instInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
    instInfo.ppEnabledExtensionNames = instanceExtensions.data();
    instInfo.enabledLayerCount = (uint32_t)instanceLayers.size();
    instInfo.ppEnabledLayerNames = instanceLayers.data();

    ERR_GUARD_VULKAN( vkCreateInstance(&instInfo, NULL, &m_VulkanInstance) );

    RegisterDebugCallbacks();

    // Find physical device

    uint32_t deviceCount = 0;
    ERR_GUARD_VULKAN( vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr) );
    assert(deviceCount > 0);

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    ERR_GUARD_VULKAN( vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, physicalDevices.data()) );

    m_PhysicalDevice = physicalDevices[0];

    // Find queue family index

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
    assert(queueFamilyCount > 0);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());
    for(uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if(queueFamilies[i].queueCount > 0)
        {
            if((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                m_GraphicsQueueFamilyIndex = i;
                break;
            }
        }
    }
    assert(m_GraphicsQueueFamilyIndex != UINT_MAX);

    // Create logical device

    const float queuePriority = 1.f;

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    deviceQueueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

    // Enable something what may interact with memory/buffer/image support.
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.fullDrawIndexUint32 = VK_TRUE;
    deviceFeatures.imageCubeArray = VK_TRUE;
    deviceFeatures.geometryShader = VK_TRUE;
    deviceFeatures.tessellationShader = VK_TRUE;
    deviceFeatures.multiDrawIndirect = VK_TRUE;
    deviceFeatures.textureCompressionBC = VK_TRUE;

    // Determine list of device extensions to enable.
    std::vector<const char*> enabledDeviceExtensions;
    //enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    {
        uint32_t propertyCount = 0;
        ERR_GUARD_VULKAN( vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, nullptr) );

        if(propertyCount)
        {
            std::vector<VkExtensionProperties> properties{propertyCount};
            ERR_GUARD_VULKAN( vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, properties.data()) );

            for(uint32_t i = 0; i < propertyCount; ++i)
            {
                if(strcmp(properties[i].extensionName, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME) == 0)
                {
                    enabledDeviceExtensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
                    VK_KHR_get_memory_requirements2_enabled = true;
                }
                else if(strcmp(properties[i].extensionName, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME) == 0)
                {
                    enabledDeviceExtensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
                    VK_KHR_dedicated_allocation_enabled = true;
                }
            }
        }
    }

    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceCreateInfo.enabledExtensionCount = (uint32_t)enabledDeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = !enabledDeviceExtensions.empty() ? enabledDeviceExtensions.data() : nullptr;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    ERR_GUARD_VULKAN( vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device) );

    // Create memory allocator

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = m_PhysicalDevice;
    allocatorInfo.device = m_Device;

    if(VK_KHR_dedicated_allocation_enabled)
    {
        allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    ERR_GUARD_VULKAN( vmaCreateAllocator(&allocatorInfo, &m_Allocator) );
}

void Player::FinalizeVulkan()
{
    if(!m_Images.empty())
    {
        printf("WARNING: Images not destroyed: %zu.\n", m_Images.size());

        for(const auto it : m_Images)
        {
            vmaDestroyImage(m_Allocator, it.second.image, it.second.allocation);
        }
    }

    if(!m_Buffers.empty())
    {
        printf("WARNING: Buffers not destroyed: %zu.\n", m_Buffers.size());

        for(const auto it : m_Buffers)
        {
            vmaDestroyBuffer(m_Allocator, it.second.buffer, it.second.allocation);
        }
    }

    if(!m_Pools.empty())
    {
        printf("WARNING: Pools not destroyed: %zu.\n", m_Pools.size());

        for(const auto it : m_Pools)
        {
            vmaDestroyPool(m_Allocator, it.second.pool);
        }
    }

    vkDeviceWaitIdle(m_Device);

    if(m_Allocator != VK_NULL_HANDLE)
    {
        vmaDestroyAllocator(m_Allocator);
        m_Allocator = nullptr;
    }

    if(m_Device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_Device, nullptr);
        m_Device = nullptr;
    }

    if(m_pvkDestroyDebugReportCallbackEXT && m_hCallback != VK_NULL_HANDLE)
    {
        m_pvkDestroyDebugReportCallbackEXT(m_VulkanInstance, m_hCallback, nullptr);
        m_hCallback = VK_NULL_HANDLE;
    }

    if(m_VulkanInstance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_VulkanInstance, NULL);
        m_VulkanInstance = VK_NULL_HANDLE;
    }
}

void Player::RegisterDebugCallbacks()
{
    m_pvkCreateDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
            (vkGetInstanceProcAddr(m_VulkanInstance, "vkCreateDebugReportCallbackEXT"));
    m_pvkDebugReportMessageEXT =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>
            (vkGetInstanceProcAddr(m_VulkanInstance, "vkDebugReportMessageEXT"));
    m_pvkDestroyDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
            (vkGetInstanceProcAddr(m_VulkanInstance, "vkDestroyDebugReportCallbackEXT"));
    assert(m_pvkCreateDebugReportCallbackEXT);
    assert(m_pvkDebugReportMessageEXT);
    assert(m_pvkDestroyDebugReportCallbackEXT);

    VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
    callbackCreateInfo.flags = //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT /*|
        VK_DEBUG_REPORT_DEBUG_BIT_EXT*/;
    callbackCreateInfo.pfnCallback = &MyDebugReportCallback;

    ERR_GUARD_VULKAN( m_pvkCreateDebugReportCallbackEXT(m_VulkanInstance, &callbackCreateInfo, nullptr, &m_hCallback) );
}

bool Player::ValidateFunctionParameterCount(size_t lineNumber, const CsvSplit& csvSplit, size_t expectedParamCount, bool lastUnbound)
{
    bool ok;
    if(lastUnbound)
        ok = csvSplit.GetCount() >= FIRST_PARAM_INDEX + expectedParamCount - 1;
    else
        ok = csvSplit.GetCount() == FIRST_PARAM_INDEX + expectedParamCount;

    if(!ok)
    {
        if(IssueWarning())
            printf("Line %zu: Incorrect number of function parameters.\n", lineNumber);
    }

    return ok;
}

void Player::CreatePool(size_t lineNumber, const CsvSplit& csvSplit)
{
    if(ValidateFunctionParameterCount(lineNumber, csvSplit, 7, false))
    {
        VmaPoolCreateInfo poolCreateInfo = {};
        uint64_t origPtr = 0;

        if(StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX), poolCreateInfo.memoryTypeIndex) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 1), poolCreateInfo.flags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 2), poolCreateInfo.blockSize) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 3), poolCreateInfo.minBlockCount) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 4), poolCreateInfo.maxBlockCount) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 5), poolCreateInfo.frameInUseCount) &&
            StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX + 6), origPtr))
        {
            Pool poolDesc = {};
            VkResult res = vmaCreatePool(m_Allocator, &poolCreateInfo, &poolDesc.pool);
            if(res == VK_SUCCESS)
            {
                const auto existingIt = m_Pools.find(origPtr);
                if(existingIt != m_Pools.end())
                {
                    if(IssueWarning())
                        printf("Line %zu: Pool %llX already exists.\n", lineNumber, origPtr);
                }
            }
            else
            {
                if(IssueWarning())
                    printf("Line %zu: vmaCreatePool failed (%u).\n", lineNumber, res);
            }
            m_Pools[origPtr] = poolDesc;
        }
        else
        {
            if(IssueWarning())
                printf("Line %zu: Invalid parameters for vmaCreatePool.\n", lineNumber);
        }
    }
}

void Player::DestroyPool(size_t lineNumber, const CsvSplit& csvSplit)
{
    if(ValidateFunctionParameterCount(lineNumber, csvSplit, 1, false))
    {
        uint64_t origPtr = 0;

        if(StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX), origPtr))
        {
            const auto it = m_Pools.find(origPtr);
            if(it != m_Pools.end())
            {
                vmaDestroyPool(m_Allocator, it->second.pool);
                m_Pools.erase(it);
            }
            else
            {
                if(IssueWarning())
                    printf("Line %zu: Pool %llX not found.\n", lineNumber, origPtr);
            }
        }
        else
        {
            if(IssueWarning())
                printf("Line %zu: Invalid parameters for vmaDestroyPool.\n", lineNumber);
        }
    }
}

void Player::CreateBuffer(size_t lineNumber, const CsvSplit& csvSplit)
{
    if(ValidateFunctionParameterCount(lineNumber, csvSplit, 12, true))
    {
        VkBufferCreateInfo bufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        VmaAllocationCreateInfo allocCreateInfo = {};
        uint64_t origPool = 0;
        uint64_t origPtr = 0;

        if(StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX), bufCreateInfo.flags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 1), bufCreateInfo.size) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 2), bufCreateInfo.usage) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 3), (uint32_t&)bufCreateInfo.sharingMode) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 4), allocCreateInfo.flags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 5), (uint32_t&)allocCreateInfo.usage) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 6), allocCreateInfo.requiredFlags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 7), allocCreateInfo.preferredFlags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 8), allocCreateInfo.memoryTypeBits) &&
            StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX + 9), origPool) &&
            StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX + 10), origPtr))
        {
            if(origPool != 0)
            {
                const auto poolIt = m_Pools.find(origPool);
                if(poolIt != m_Pools.end())
                    allocCreateInfo.pool = poolIt->second.pool;
                else
                {
                    if(IssueWarning())
                        printf("Line %zu: Pool %llX not found.\n", lineNumber, origPool);
                }
            }

            Buffer bufDesc = {};
            VkResult res = vmaCreateBuffer(m_Allocator, &bufCreateInfo, &allocCreateInfo, &bufDesc.buffer, &bufDesc.allocation, nullptr);
            if(res == VK_SUCCESS)
            {
                const auto existingIt = m_Buffers.find(origPtr);
                if(existingIt != m_Buffers.end())
                {
                    if(IssueWarning())
                        printf("Line %zu: Allocation %llX for buffer already exists.\n", lineNumber, origPtr);
                }
            }
            else
            {
                if(IssueWarning())
                    printf("Line %zu: vmaCreateBuffer failed (%u).\n", lineNumber, res);
            }
            m_Buffers[origPtr] = bufDesc;
        }
        else
        {
            if(IssueWarning())
                printf("Line %zu: Invalid parameters for vmaCreateBuffer.\n", lineNumber);
        }
    }
}

void Player::DestroyBuffer(size_t lineNumber, const CsvSplit& csvSplit)
{
    if(ValidateFunctionParameterCount(lineNumber, csvSplit, 1, false))
    {
        uint64_t origAllocPtr = 0;

        if(StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX), origAllocPtr))
        {
            const auto it = m_Buffers.find(origAllocPtr);
            if(it != m_Buffers.end())
            {
                vmaDestroyBuffer(m_Allocator, it->second.buffer, it->second.allocation);
                m_Buffers.erase(it);
            }
            else
            {
                if(IssueWarning())
                    printf("Line %zu: Allocation %llX for buffer not found.\n", lineNumber, origAllocPtr);
            }
        }
        else
        {
            if(IssueWarning())
                printf("Line %zu: Invalid parameters for vmaDestroyBuffer.\n", lineNumber);
        }
    }
}

void Player::CreateImage(size_t lineNumber, const CsvSplit& csvSplit)
{
    if(ValidateFunctionParameterCount(lineNumber, csvSplit, 21, true))
    {
        VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        VmaAllocationCreateInfo allocCreateInfo = {};
        uint64_t origPool = 0;
        uint64_t origPtr = 0;

        if(StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX), imageCreateInfo.flags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 1), (uint32_t&)imageCreateInfo.imageType) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 2), (uint32_t&)imageCreateInfo.format) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 3), imageCreateInfo.extent.width) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 4), imageCreateInfo.extent.height) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 5), imageCreateInfo.extent.depth) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 6), imageCreateInfo.mipLevels) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 7), imageCreateInfo.arrayLayers) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 8), (uint32_t&)imageCreateInfo.samples) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 9), (uint32_t&)imageCreateInfo.tiling) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 10), imageCreateInfo.usage) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 11), (uint32_t&)imageCreateInfo.sharingMode) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 12), (uint32_t&)imageCreateInfo.initialLayout) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 13), allocCreateInfo.flags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 14), (uint32_t&)allocCreateInfo.usage) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 15), allocCreateInfo.requiredFlags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 16), allocCreateInfo.preferredFlags) &&
            StrRangeToUint(csvSplit.GetRange(FIRST_PARAM_INDEX + 17), allocCreateInfo.memoryTypeBits) &&
            StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX + 18), origPool) &&
            StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX + 19), origPtr))
        {
            if(origPool != 0)
            {
                const auto poolIt = m_Pools.find(origPool);
                if(poolIt != m_Pools.end())
                    allocCreateInfo.pool = poolIt->second.pool;
                else
                {
                    if(IssueWarning())
                        printf("Line %zu: Pool %llX not found.\n", lineNumber, origPool);
                }
            }

            Image imageDesc = {};
            VkResult res = vmaCreateImage(m_Allocator, &imageCreateInfo, &allocCreateInfo, &imageDesc.image, &imageDesc.allocation, nullptr);
            if(res == VK_SUCCESS)
            {
                const auto existingIt = m_Images.find(origPtr);
                if(existingIt != m_Images.end())
                {
                    if(IssueWarning())
                        printf("Line %zu: Allocation %llX for image already exists.\n", lineNumber, origPtr);
                }
            }
            else
            {
                if(IssueWarning())
                    printf("Line %zu: vmaCreateImage failed (%u).\n", lineNumber, res);
            }
            m_Images[origPtr] = imageDesc;
        }
        else
        {
            if(IssueWarning())
                printf("Line %zu: Invalid parameters for vmaCreateImage.\n", lineNumber);
        }
    }
}

void Player::DestroyImage(size_t lineNumber, const CsvSplit& csvSplit)
{
    if(ValidateFunctionParameterCount(lineNumber, csvSplit, 1, false))
    {
        uint64_t origAllocPtr = 0;

        if(StrRangeToPtr(csvSplit.GetRange(FIRST_PARAM_INDEX), origAllocPtr))
        {
            const auto it = m_Images.find(origAllocPtr);
            if(it != m_Images.end())
            {
                vmaDestroyImage(m_Allocator, it->second.image, it->second.allocation);
                m_Images.erase(it);
            }
            else
            {
                if(IssueWarning())
                    printf("Line %zu: Allocation %llX for image not found.\n", lineNumber, origAllocPtr);
            }
        }
        else
        {
            if(IssueWarning())
                printf("Line %zu: Invalid parameters for vmaDestroyBuffer.\n", lineNumber);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// Main functions

static void PrintCommandLineSyntax()
{
    printf("Command line syntax:\n"
        "    VmaReplay <SrcFile.csv>\n");
}

static int ProcessFile(const char* data, size_t numBytes)
{
    // Begin stats.
    printf("File size: %zu B\n", numBytes);

    LineSplit lineSplit(data, numBytes);
    StrRange line;

    if(!lineSplit.GetNextLine(line) ||
        !StrRangeEq(line, "Vulkan Memory Allocator,Calls recording"))
    {
        printf("ERROR: Incorrect file format.\n");
        return RESULT_ERROR_FORMAT;
    }

    if(!lineSplit.GetNextLine(line) ||
        !StrRangeEq(line, "1,0"))
    {
        printf("ERROR: Incorrect file format version.\n");
        return RESULT_ERROR_FORMAT;
    }

    Player player;
    player.Init();

    printf("Playing...\n");
    while(lineSplit.GetNextLine(line))
    {
        player.ExecuteLine(lineSplit.GetNextLineIndex(), line);
    }

    // End stats.
    printf("Done.\n");
    printf("File lines: %zu\n", lineSplit.GetNextLineIndex());

    return 0;
}

static int ProcessFile(const char* filePath)
{
    printf("Replaying file \"%s\"...\n", filePath);
    int result = 0;

    FILE* file = nullptr;
    const errno_t err = fopen_s(&file, filePath, "rb");
    if(err == 0)
    {
        _fseeki64(file, 0, SEEK_END);
        const size_t fileSize = (size_t)_ftelli64(file);
        _fseeki64(file, 0, SEEK_SET);

        if(fileSize > 0)
        {
            std::vector<char> fileContents(fileSize);
            fread(fileContents.data(), 1, fileSize, file);
            ProcessFile(fileContents.data(), fileContents.size());
        }
        else
        {
            printf("ERROR: Source file is empty.\n");
            result = RESULT_ERROR_SOURCE_FILE;
        }

        fclose(file);
    }
    else
    {
        printf("ERROR: Couldn't open file (%i).\n", err);
        result = RESULT_ERROR_SOURCE_FILE;
    }

    return result;
}

static int main2(int argc, char** argv)
{
    if(argc != 2)
    {
        PrintCommandLineSyntax();
        return RESULT_ERROR_COMMAND_LINE;
    }

    return ProcessFile(argv[1]);
}

int main(int argc, char** argv)
{
    try
    {
        main2(argc, argv);
    }
    catch(const std::exception& e)
    {
        printf("ERROR: %s\n", e.what());
        return RESULT_EXCEPTION;
    }
    catch(...)
    {
        printf("UNKNOWN ERROR\n");
        return RESULT_EXCEPTION;
    }

    return 0;
}
