#include "VulkronInternal.h"

SwapchainInternal*			swapchainInternal			= new SwapchainInternal();
SwapchainSupportDetails*	swapchainSupportDetails		= new SwapchainSupportDetails();

// SwapChain
static SwapchainSupportDetails querySwapChainSupport();

VulkronResult vulkronCreateSwapchain(VulkronSwapchainCreateInfo* info) {

    if (nullptr == info) {
        return VULKRON_ERROR_INVALID_ARGUMENT;
    }

    createSwapchain(info->width, info->height, info->vsync);

    return VULKRON_SUCCESS;
}

void destroySwapchain() {
    cleanUpSwapchain();

    delete swapchainInternal;
    delete swapchainSupportDetails;
}

//-------------------------------------------------------------------------------------
// SECTION [SWAPCHAIN] ----------------------------------------------------------------
//-------------------------------------------------------------------------------------

void cleanUpSwapchain() {
    if (swapchainInternal->swapChain != VULKRON_NULL_HANDLE) {
        for (uint32_t i = 0; i < swapchainInternal->imageCount; i++) {
            vkDestroyImageView(deviceInternal->logicalDevice, swapchainInternal->bufferList[i].view, nullptr);
        }
    }

    if (instanceInternal->surface != VULKRON_NULL_HANDLE) {
        vkDestroySwapchainKHR(deviceInternal->logicalDevice, swapchainInternal->swapChain, nullptr);
        vkDestroySurfaceKHR(*instance->pInstance, instanceInternal->surface, nullptr);
    }

    instanceInternal->surface = VULKRON_NULL_HANDLE;
    swapchainInternal->swapChain = VULKRON_NULL_HANDLE;
}

void createSwapchain(uint32_t* width, uint32_t* height, bool vsync) {

    swapchainInternal->vsync = vsync;

    SwapchainSupportDetails swapChainSupport = querySwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = {};
    for (const auto& availableFormat : swapChainSupport.formatList) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = availableFormat;
        }
    }

    swapchainInternal->swapChainImageFormat = surfaceFormat.format;

    VkPresentModeKHR presentMode = {};
    if (!swapchainInternal->vsync) {
        for (const auto& availablePresentMode : swapChainSupport.presentModeList) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                presentMode = availablePresentMode;
            }
        }
    }
    else {
        presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }


    if (swapChainSupport.capabilities.currentExtent.width != 0xFFFFFFFF) { // Is undefined
        swapchainInternal->swapChainExtent = swapChainSupport.capabilities.currentExtent;

    }
    else if (swapChainSupport.capabilities.currentExtent.width == 0xFFFFFFFF) { // Is defined
        swapchainInternal->swapChainExtent.width = *width;
        swapchainInternal->swapChainExtent.height = *height;
    }

    uint32_t numberOfImages = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && numberOfImages > swapChainSupport.capabilities.maxImageCount) {
        numberOfImages = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainKHR oldSwapchain = swapchainInternal->swapChain;

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = instanceInternal->surface;
    swapchainCreateInfo.minImageCount = numberOfImages;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchainInternal->swapChainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    if (vkCreateSwapchainKHR(deviceInternal->logicalDevice, &swapchainCreateInfo, nullptr, &swapchainInternal->swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    if (oldSwapchain != VULKRON_NULL_HANDLE) {
        for (uint32_t i = 0; i < swapchainInternal->imageCount; i++) {
            vkDestroyImageView(deviceInternal->logicalDevice, swapchainInternal->bufferList[i].view, nullptr);
        }
        vkDestroySwapchainKHR(deviceInternal->logicalDevice, oldSwapchain, nullptr);
    }
}

static SwapchainSupportDetails querySwapChainSupport() {
    SwapchainSupportDetails details;

    VkSurfaceKHR surface = instanceInternal->surface;
    VkPhysicalDevice gpu = deviceInternal->gpu;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formatList.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, details.formatList.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModeList.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, details.presentModeList.data());
    }

    return details;
}