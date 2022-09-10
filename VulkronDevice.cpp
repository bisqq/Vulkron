#include "VulkronInternal.h"

VulkronDeviceCreateInfo*    device			= new VulkronDeviceCreateInfo();
DeviceInternal*		        deviceInternal	= new DeviceInternal();
QueueFamily*		        queueFamily		= new QueueFamily();
Queue*                      queue           = new Queue();

// Device
static void userPickGpu();
static void pickMostEfficientGpu();
static void createLogicalDevice();
static VkPhysicalDeviceFeatures* getFeatures(VulkronGpuFeatures features);
static int rateGpuSuitability(VkPhysicalDevice gpu);
static void getSupportedDeviceExtensions();
static uint32_t findQueueFamilies(VkQueueFlagBits queueFlag);
static void getGpuProperties();

VulkronResult vulkronCreateDevice(VulkronDeviceCreateInfo* info) {
    device = info;

    if (nullptr == device) {
        return VULKRON_ERROR_MEMORY_ALLOCATE;
    }

    if (nullptr == device->gpuList.data() || device->gpuList.size() == 0) {
        return VULKRON_ERROR_INVALID_ARGUMENT;
    }

    if (nullptr != device->gpuList.data() && device->gpuList.size() == 1) {
        userPickGpu();
    }
    else if (nullptr != device->gpuList.data()) {
        pickMostEfficientGpu();
    }

    createLogicalDevice();

    return VULKRON_SUCCESS;
}

void destroyDevice() {
    delete device;
    delete deviceInternal;
    delete queueFamily;
}


//-------------------------------------------------------------------------------------
// SECTION [DEVICE] -------------------------------------------------------------------
//-------------------------------------------------------------------------------------


static void userPickGpu() {

    // get selected gpu features geometry shader to make sure application runs
    VkPhysicalDeviceFeatures gpuFeatures;
    vkGetPhysicalDeviceFeatures(device->pGpu[0], &gpuFeatures);

    if (!gpuFeatures.geometryShader) {
        throw std::runtime_error("Gpu doesn't support geometry shaders");
    }

    deviceInternal->gpu = device->pGpu[0];

    getGpuProperties();
}

static void pickMostEfficientGpu() {

    std::multimap<int, VkPhysicalDevice> candidates;

    for (uint32_t i = 0; i < device->gpuCount; i++) {
        uint32_t score = rateGpuSuitability(device->pGpu[i]);
        candidates.insert(std::make_pair(score, device->pGpu[i]));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        deviceInternal->gpu = candidates.rbegin()->second;
    }
    else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    getGpuProperties();

}

static void createLogicalDevice() {

    float defaultQueuePriority = 0.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    if (device->graphicsQueueCount == 0) {
        throw std::runtime_error("queue count of 0 is not allowed");
    }

    // https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanDevice.cpp
    // Graphics queue
    if (device->graphicsQueueFlag & VK_QUEUE_GRAPHICS_BIT) {
        deviceInternal->queuefamily.graphicsQueueIndex = findQueueFamilies(VK_QUEUE_GRAPHICS_BIT);
        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = deviceInternal->queuefamily.graphicsQueueIndex;
        queueInfo.queueCount = device->graphicsQueueCount;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
    }
    else {
        deviceInternal->queuefamily.graphicsQueueIndex = 0;
    }

    // Dedicated compute queue
    if (device->computeQueueFlag & VK_QUEUE_COMPUTE_BIT) {
        deviceInternal->queuefamily.computeQueueIndex = findQueueFamilies(VK_QUEUE_COMPUTE_BIT);

        if (deviceInternal->queuefamily.computeQueueIndex != deviceInternal->queuefamily.graphicsQueueIndex) {
            // If compute family index differs, we need an additional queue create info for the compute queue
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = deviceInternal->queuefamily.computeQueueIndex;
            queueInfo.queueCount = device->computeQueueCount;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    }
    else {
        // Else we use the same queue
        deviceInternal->queuefamily.computeQueueIndex = deviceInternal->queuefamily.graphicsQueueIndex;
    }

    // Dedicated transfer queue
    if (device->transferQueueFlag & VK_QUEUE_TRANSFER_BIT) {
        deviceInternal->queuefamily.transferQueueIndex = findQueueFamilies(VK_QUEUE_TRANSFER_BIT);

        if ((deviceInternal->queuefamily.transferQueueIndex != deviceInternal->queuefamily.graphicsQueueIndex)
            && (deviceInternal->queuefamily.transferQueueIndex != deviceInternal->queuefamily.computeQueueIndex)) {
            // If compute family index differs, we need an additional queue create info for the compute queue
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = deviceInternal->queuefamily.transferQueueIndex;
            queueInfo.queueCount = device->transferQueueCount;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    }
    else {
        // Else we use the same queue
        deviceInternal->queuefamily.transferQueueIndex = deviceInternal->queuefamily.graphicsQueueIndex;
    }

    VkPhysicalDeviceFeatures* features = getFeatures(device->gpuEnabledFeatures);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = features;

    std::vector<const char*> deviceExtensions;

    // If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
    if (device->isUsingSwapchain) {
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

#ifdef _DEBUG
    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instanceInternal->validationLayersList.size());
    deviceCreateInfo.ppEnabledLayerNames = instanceInternal->validationLayersList.data();
#else
    createInfo.enabledLayerCount = 0;
#endif // _DEBUG

    if (vkCreateDevice(deviceInternal->gpu, &deviceCreateInfo, nullptr, &deviceInternal->logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(deviceInternal->logicalDevice, deviceInternal->queuefamily.graphicsQueueIndex, 0, &queue->graphics);
    vkGetDeviceQueue(deviceInternal->logicalDevice, deviceInternal->queuefamily.computeQueueIndex, 0, &queue->compute);
    vkGetDeviceQueue(deviceInternal->logicalDevice, deviceInternal->queuefamily.transferQueueIndex, 0, &queue->transfer);

    delete features;
}

static VkPhysicalDeviceFeatures* getFeatures(VulkronGpuFeatures features) {
    VkPhysicalDeviceFeatures* vkFeatures = new VkPhysicalDeviceFeatures();

    vkFeatures->robustBufferAccess = features.robustBufferAccess;
    vkFeatures->fullDrawIndexUint32 = features.fullDrawIndexUint32;
    vkFeatures->imageCubeArray = features.imageCubeArray;
    vkFeatures->independentBlend = features.independentBlend;
    vkFeatures->geometryShader = features.geometryShader;
    vkFeatures->tessellationShader = features.tessellationShader;
    vkFeatures->sampleRateShading = features.sampleRateShading;
    vkFeatures->dualSrcBlend = features.dualSrcBlend;
    vkFeatures->logicOp = features.logicOp;
    vkFeatures->multiDrawIndirect = features.multiDrawIndirect;
    vkFeatures->drawIndirectFirstInstance = features.drawIndirectFirstInstance;
    vkFeatures->depthClamp = features.depthClamp;
    vkFeatures->depthBiasClamp = features.depthBiasClamp;
    vkFeatures->fillModeNonSolid = features.fillModeNonSolid;
    vkFeatures->depthBounds = features.depthBounds;
    vkFeatures->wideLines = features.wideLines;
    vkFeatures->largePoints = features.largePoints;
    vkFeatures->alphaToOne = features.alphaToOne;
    vkFeatures->multiViewport = features.multiViewport;
    vkFeatures->samplerAnisotropy = features.samplerAnisotropy;
    vkFeatures->textureCompressionETC2 = features.textureCompressionETC2;
    vkFeatures->textureCompressionASTC_LDR = features.textureCompressionASTC_LDR;
    vkFeatures->textureCompressionBC = features.textureCompressionBC;
    vkFeatures->occlusionQueryPrecise = features.occlusionQueryPrecise;
    vkFeatures->pipelineStatisticsQuery = features.pipelineStatisticsQuery;
    vkFeatures->vertexPipelineStoresAndAtomics = features.vertexPipelineStoresAndAtomics;
    vkFeatures->fragmentStoresAndAtomics = features.fragmentStoresAndAtomics;
    vkFeatures->shaderTessellationAndGeometryPointSize = features.shaderTessellationAndGeometryPointSize;
    vkFeatures->shaderImageGatherExtended = features.shaderImageGatherExtended;
    vkFeatures->shaderStorageImageExtendedFormats = features.shaderStorageImageExtendedFormats;
    vkFeatures->shaderStorageImageMultisample = features.shaderStorageImageMultisample;
    vkFeatures->shaderStorageImageReadWithoutFormat = features.shaderStorageImageReadWithoutFormat;
    vkFeatures->shaderStorageImageWriteWithoutFormat = features.shaderStorageImageWriteWithoutFormat;
    vkFeatures->shaderUniformBufferArrayDynamicIndexing = features.shaderUniformBufferArrayDynamicIndexing;
    vkFeatures->shaderSampledImageArrayDynamicIndexing = features.shaderSampledImageArrayDynamicIndexing;
    vkFeatures->shaderStorageBufferArrayDynamicIndexing = features.shaderStorageBufferArrayDynamicIndexing;
    vkFeatures->shaderStorageImageArrayDynamicIndexing = features.shaderStorageImageArrayDynamicIndexing;
    vkFeatures->shaderClipDistance = features.shaderClipDistance;
    vkFeatures->shaderCullDistance = features.shaderCullDistance;
    vkFeatures->shaderFloat64 = features.shaderFloat64;
    vkFeatures->shaderInt64 = features.shaderInt64;
    vkFeatures->shaderInt16 = features.shaderInt16;
    vkFeatures->shaderResourceResidency = features.shaderResourceResidency;
    vkFeatures->shaderResourceMinLod = features.shaderResourceMinLod;
    vkFeatures->sparseBinding = VULKRON_FALSE;
    vkFeatures->sparseResidencyBuffer = VULKRON_FALSE;
    vkFeatures->sparseResidencyImage2D = VULKRON_FALSE;
    vkFeatures->sparseResidencyImage3D = VULKRON_FALSE;
    vkFeatures->sparseResidency2Samples = VULKRON_FALSE;
    vkFeatures->sparseResidency4Samples = VULKRON_FALSE;
    vkFeatures->sparseResidency8Samples = VULKRON_FALSE;
    vkFeatures->sparseResidency16Samples = VULKRON_FALSE;
    vkFeatures->sparseResidencyAliased = VULKRON_FALSE;
    vkFeatures->variableMultisampleRate = features.variableMultisampleRate;
    vkFeatures->inheritedQueries = features.inheritedQueries;

    return vkFeatures;
}

static int rateGpuSuitability(VkPhysicalDevice gpu) {
    int score = 0;

    VkPhysicalDeviceProperties gpuProperties;
    vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

    VkPhysicalDeviceFeatures gpuFeatures;
    vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);

    // Discrete GPUs have a significant performance advantage
    if (gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += gpuProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!gpuFeatures.geometryShader) {
        return 0;
    }

    return score;
}

static void getSupportedDeviceExtensions() {

    // Get all supported extensions
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(deviceInternal->gpu, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(deviceInternal->gpu, nullptr, &extensionCount, availableExtensions.data());

    for (const auto& extension : availableExtensions) {
        deviceInternal->supportedExtensionsList.push_back(extension.extensionName);
    }
}

static uint32_t findQueueFamilies(VkQueueFlagBits queueFlag) {

    uint32_t queueFamilySize = static_cast<uint32_t>(deviceInternal->queuefamily.queueFamilyPropertiesList.size());

    switch (queueFlag) {

    case VK_QUEUE_GRAPHICS_BIT:
        for (uint32_t i = 0; i < queueFamilySize; i++) {

            VkQueueFlags isGraphicsBit = deviceInternal->queuefamily.queueFamilyPropertiesList[i].queueFlags & queueFlag;

            if (isGraphicsBit) {
                return i;
            }
        }
        break;

    case VK_QUEUE_COMPUTE_BIT:
        for (uint32_t i = 0; i < queueFamilySize; i++) {

            VkQueueFlags isComputeBit = deviceInternal->queuefamily.queueFamilyPropertiesList[i].queueFlags & queueFlag;
            VkQueueFlags isNotGraphicsBit = ((deviceInternal->queuefamily.queueFamilyPropertiesList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0);

            if (isComputeBit && isNotGraphicsBit) {
                return i;
            }
        }
        break;

    case VK_QUEUE_TRANSFER_BIT:
        for (uint32_t i = 0; i < queueFamilySize; i++) {

            VkQueueFlags isTransferBit = deviceInternal->queuefamily.queueFamilyPropertiesList[i].queueFlags & queueFlag;
            VkQueueFlags isNotGraphgicsBit = ((deviceInternal->queuefamily.queueFamilyPropertiesList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0);
            VkQueueFlags isNoteComputeBit = ((deviceInternal->queuefamily.queueFamilyPropertiesList[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0);

            if (isTransferBit && isNotGraphgicsBit && isNoteComputeBit) {
                return i;
            }
        }
        break;

    default:
        throw std::runtime_error("Could not find a matching queue family index");
        break;
    }
}

static void getGpuProperties() {
    vkGetPhysicalDeviceProperties(deviceInternal->gpu, &deviceInternal->gpuProperties);
    vkGetPhysicalDeviceMemoryProperties(deviceInternal->gpu, &deviceInternal->gpuMemoryProperties);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(deviceInternal->gpu, &queueFamilyCount, nullptr);

    deviceInternal->queuefamily.queueFamilyPropertiesList.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(deviceInternal->gpu, &queueFamilyCount, deviceInternal->queuefamily.queueFamilyPropertiesList.data());

    getSupportedDeviceExtensions();
}