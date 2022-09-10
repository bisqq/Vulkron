#pragma once

#include "VulkronCore.h"

#include <map>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdint.h>
#include <memory>
#include <array>

void destroyInstance();
void destroyDevice();
void destroySwapchain();
void destroyPipeline();
void destroyFrameBuffer();
void destroyCommands();

void createSwapchain(uint32_t* width, uint32_t* height, bool vsync);
void cleanUpSwapchain();
void createRenderPass(VulkronAttachmentFlags flag);
void createGraphicsPipeline();

struct QueueFamily;
struct Queue;
struct SwapchainSupportDetails;
struct SwapchainBuffers;
struct ThreadData;
struct CommandBufferData;

struct InstanceInternal;
struct DeviceInternal;
struct SwapchainInternal;
struct RenderPassInternal;
struct DrawInternal;

typedef std::unordered_map<uint32_t, std::vector<ThreadData>> threadDataMap;

extern VulkronInstanceCreateInfo*           instance;
extern InstanceInternal*                    instanceInternal;
extern VulkronDeviceCreateInfo*             device;
extern DeviceInternal*                      deviceInternal;
extern QueueFamily*                         queueFamily;
extern Queue*                               queue;
extern SwapchainInternal*                   swapchainInternal;
extern SwapchainSupportDetails*             swapchainSupportDetails;
extern VulkronGraphicsPipelineCreateInfo*   pipeline;
extern RenderPassInternal*                  renderPassInternal;
extern CommandBufferData*                   commandBufferData;
extern DrawInternal*                        drawInternal;

extern const uint32_t                       MAX_FRAMES_IN_FLIGHT;
extern VkCommandPool                        primaryCommandPool;


// ---------------------------------- 
// Internal Ext Structs -------------
// ---------------------------------- 

typedef struct QueueFamily {
    std::vector<VkQueueFamilyProperties>	queueFamilyPropertiesList;		// Family of VKQueues (graphics, compute, transfer, sparse queues). A queue family just describes a set of queues with identical properties
    uint32_t								graphicsQueueIndex;				// Graphics queues run graphics pipelines started by vkCmdDraw* commands
    uint32_t								computeQueueIndex;				// Compute queues run compute pipelines started by vkCmdDispatch*
    uint32_t								transferQueueIndex;				// Transfer queues can perform transfer (copy) operations from vkCmdCopy*
} QueueFamily;

typedef struct Queue {
    VkQueue                                 graphics;
    VkQueue                                 compute;
    VkQueue                                 transfer;
} Queue;

typedef struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR				capabilities;
    std::vector<VkSurfaceFormatKHR>			formatList;
    std::vector<VkPresentModeKHR>			presentModeList;
} SwapchainSupportDetails;

typedef struct SwapchainBuffers {
    VkImage									image;
    VkImageView								view;
    VkFramebuffer                           frameBuffer;
} SwapchainBuffers;

typedef struct ThreadData {
    uint32_t                                imageIndex;                     // track to reset frame pool
    VkCommandPool                           commandPool;                    // command pool per frame
    std::vector<VkCommandBuffer>            secondarydynamicBufferList;     // Dynamic command buffers, used for batches
} ThreadData;

typedef struct CommandBufferData {
    threadDataMap                           threadBuffersMap;               // dynamic command buffers
    VkCommandBuffer                         primaryBuffer;
    VkCommandBuffer                         secondaryStaticBuffer;
    std::vector<VulkronBaseObject>          staticObjectsList;              // static objects to draw on screen
    std::vector<VulkronBaseObject>          dynamicObjectsList;             // dynamic objects to draw on screen
} CommandBufferData;

// ---------------------------------- 
// Internal Structs -----------------
// ---------------------------------- 

typedef struct InstanceInternal {
    VkSurfaceKHR							surface;
    const std::vector<const char*>          validationLayersList = { "VK_LAYER_KHRONOS_validation" };
} InstanceInternal;

typedef struct DeviceInternal {
    VkPhysicalDevice						gpu;					        // Choose specific GPU
    VkDevice								logicalDevice;					// applications view of the device
    VkPhysicalDeviceProperties				gpuProperties;					// The properties of the GPU, that includes the limits that the application can check against
    VkPhysicalDeviceMemoryProperties		gpuMemoryProperties;			// Memory types and heaps of the physical device
    std::vector<const char*>				supportedExtensionsList;		// logical device supported extensions
    QueueFamily								queuefamily;
} DeviceInternal;

typedef struct SwapchainInternal {
    bool                                    vsync;
    VkSwapchainKHR							swapChain;
    std::vector<VkImage>					swapChainImagesList;
    VkExtent2D								swapChainExtent;
    uint32_t								imageCount;
    VkFormat								swapChainImageFormat;
    std::vector<SwapchainBuffers>			bufferList;
} SwapchainInternal;

typedef struct RenderPassInternal {
    VulkronAttachmentFlags                  flag;
    std::vector<VkAttachmentDescription>	attachmentsList;
    std::vector<VkSubpassDependency>		dependencyList;
    std::vector<VkSubpassDescription>		subpassList;
} RenderPassInternal;

typedef struct DrawInternal {
    std::vector<VkFramebuffer>				swapChainFrameBuffersList;
    std::vector<VkSemaphore>				imageAvailableSemaphores;	    // Acquire an image
    std::vector<VkSemaphore>				renderFinishedSemaphores;	    // Present an image
    std::vector<VkFence>					inFlightFences;
    std::vector<VkFence>					imagesInFlight;
} DrawInternal;


