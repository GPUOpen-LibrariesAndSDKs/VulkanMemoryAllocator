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
    void GetRange(size_t index, const char*& outBeg, const char*& outEnd) const 
    {
        outBeg = m_Str + m_Ranges[index * 2];
        outEnd = m_Str + m_Ranges[index * 2 + 1];
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

class Player
{
public:
    Player();
    void Init();
    ~Player();

    void ExecuteLine(size_t lineNumber, const StrRange& line);

private:
    static const size_t MAX_WARNINGS_TO_SHOW = 16;

    // Increments warning counter. Returns true if warning message should be printed.
    bool IssueWarning() { return m_WarningCount++ < MAX_WARNINGS_TO_SHOW; }

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

    void InitVulkan();
    void FinalizeVulkan();
    void RegisterDebugCallbacks();
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

    if(m_WarningCount > 0)
        printf("WARNING: %zu more warnings not shown.\n", m_WarningCount - MAX_WARNINGS_TO_SHOW);
}

void Player::ExecuteLine(size_t lineNumber, const StrRange& line)
{
    CsvSplit csvSplit;
    csvSplit.Set(line);

    if(csvSplit.GetCount() >= 4)
    {
        // TODO
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
    enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
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

    VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
    callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    callbackCreateInfo.pNext = nullptr;
    callbackCreateInfo.flags = //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT /*|
        VK_DEBUG_REPORT_DEBUG_BIT_EXT*/;
    callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
    callbackCreateInfo.pUserData   = nullptr;

    ERR_GUARD_VULKAN( m_pvkCreateDebugReportCallbackEXT(m_VulkanInstance, &callbackCreateInfo, nullptr, &m_hCallback) );
}

////////////////////////////////////////////////////////////////////////////////
// Main functions

static void PrintCommandLineSyntax()
{
    printf("Command line syntax:\n"
        "    VmaReplay <SrcFile.csv>\n");
}

static inline bool StrRangeEq(const StrRange& lhs, const char* rhsSz)
{
    const size_t rhsLen = strlen(rhsSz);
    return rhsLen == lhs.length() &&
        memcmp(lhs.beg, rhsSz, rhsLen) == 0;
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

    while(lineSplit.GetNextLine(line))
    {
        player.ExecuteLine(lineSplit.GetNextLineIndex(), line);
    }

    // End stats.
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

    printf("Done.\n");

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
