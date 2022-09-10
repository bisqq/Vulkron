#include "VulkronInternal.h"

VulkronInstanceCreateInfo*      instance            = new VulkronInstanceCreateInfo();
InstanceInternal*               instanceInternal    = new InstanceInternal();

// Instance
static void createApplicationInstance();
static void createGlfwWindowSurface();
static bool checkValidationLayerSupport(VulkronInstanceCreateInfo* info, const std::vector<const char*> validationLayersList);
static VKAPI_ATTR VulkronBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

VulkronResult vulkronCreateInstance(VulkronInstanceCreateInfo* info) {
    instance = info;

    if (nullptr == instance) {
        return VULKRON_ERROR_MEMORY_ALLOCATE;
    }

    createApplicationInstance();
    createGlfwWindowSurface();

    return VULKRON_SUCCESS;
}

void destroyInstance() {
    delete instance;
    delete instanceInternal;
}

VulkronResult vulkronShutdown() {
    vkDeviceWaitIdle(deviceInternal->logicalDevice);
    cleanUpSwapchain();
    //---------------------------------------------

    //vkDestroySampler(device, textureSampler, nullptr);
    //vkDestroyImageView(device, textureImageView, nullptr);

    //vkDestroyImage(device, textureImage, nullptr);
    //vkFreeMemory(device, textureImageMemory, nullptr);

    //vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    //vkDestroyBuffer(device, indexBuffer, nullptr);
    //vkFreeMemory(device, indexBufferMemory, nullptr);

    //vkDestroyBuffer(device, vertexBuffer, nullptr);
    //vkFreeMemory(device, vertexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(deviceInternal->logicalDevice, drawInternal->renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(deviceInternal->logicalDevice, drawInternal->imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(deviceInternal->logicalDevice, drawInternal->inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(deviceInternal->logicalDevice, primaryCommandPool, nullptr);

    vkDestroyDevice(deviceInternal->logicalDevice, nullptr);

    //if (enableValidationLayers) { 
    //    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr); 
    //}

    vkDestroySurfaceKHR(*instance->pInstance, instanceInternal->surface, nullptr);
    vkDestroyInstance(*instance->pInstance, nullptr);

    return VULKRON_SUCCESS;
}


//-------------------------------------------------------------------------------------
//	SECTION [INSTANCE] ----------------------------------------------------------------
//-------------------------------------------------------------------------------------

static void createApplicationInstance() {
#ifdef _DEBUG
    if (!checkValidationLayerSupport(instance, instanceInternal->validationLayersList)) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
#endif // _DEBUG

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "Vulkron";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "Vulkron Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    uint32_t extensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);

#ifdef _DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // _DEBUG

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.enabledLayerCount = NULL;
    instanceCreateInfo.pNext = nullptr;


#ifdef _DEBUG
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instanceInternal->validationLayersList.size());
    instanceCreateInfo.ppEnabledLayerNames = instanceInternal->validationLayersList.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    populateDebugMessengerCreateInfo(debugCreateInfo);
    instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

#endif // _DEBUG

    if (vkCreateInstance(&instanceCreateInfo, nullptr, instance->pInstance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
    populateDebugMessengerCreateInfo(debugUtilsMessengerCreateInfoEXT);

    VkDebugUtilsMessengerEXT debugMessenger;
    if (CreateDebugUtilsMessengerEXT(*instance->pInstance, &debugUtilsMessengerCreateInfoEXT, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

static void createGlfwWindowSurface() {
    if (glfwCreateWindowSurface(*instance->pInstance, instance->pWindow, nullptr, &instanceInternal->surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

static bool checkValidationLayerSupport(VulkronInstanceCreateInfo* info, const std::vector<const char*> validationLayersList) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayersList) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VULKRON_FALSE;
}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}