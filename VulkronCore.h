#pragma once

#ifndef VULKRON_CORE
#define VULKRON_CORE

#include "vulkan/vulkan.h"
#include "Glfw/glfw3.h"
#include "glm/glm.hpp"

#include <vector>
#include <string>

// Macros for specific target platform exporting
#ifdef _WIN64
#define VULKRON_PRE _declspec(dllexport)
#define VULKRON_POST __cdecl
#elif __linux__
#define VULKRON_PRE 
#define VULKRON_POST
#elif __APPLE__
#define VULKRON_PRE 
#define VULKRON_POST
#endif

#define VULKRON_VERSION 0.1
#define VULKRON_NULL_HANDLE				VK_NULL_HANDLE
#define VULKRON_NULLPTR_HANDLE			nullptr
#define VULKRON_FALSE					VK_FALSE
#define VULKRON_DEFINE_U32TYPE(type)	typedef uint32_t type;

#if defined _DEBUG || defined VULKRON_ENGINE_DEBUGGING
	#define LOG(x) std::cout << x << std::endl;
#endif // _DEBUG || VULKRON_ENGINE_DEBUGGING

VULKRON_DEFINE_U32TYPE(VulkronFlags)
VULKRON_DEFINE_U32TYPE(VulkronBool32)

typedef enum VulkronResult {
	VULKRON_SUCCESS = 0,
	VULKRON_SUCCESS_MEMORY_DEALLOCATED = 1,
	VULKRON_ERROR_MEMORY_ALLOCATE = -1,
	VULKRON_ERROR_INVALID_ARGUMENT = -1
} VulkronResult;

typedef enum VulkronMemoryUsage {
	VULKRON_MEMORY_USAGE_GPU_STORAGE = 0,
	VULKRON_MEMORY_USAGE_CPU_VISIBLE,
	VULKRON_MEMORY_USAGE_CPU_COHERENT,
	VULKRON_MEMORY_USAGE_CPU_CACHED,
	VULKRON_MEMORY_USAGE_UPLOAD_ONCE = VULKRON_MEMORY_USAGE_GPU_STORAGE,
	VULKRON_MEMORY_USAGE_STAGING_TO_VRAM = VULKRON_MEMORY_USAGE_CPU_VISIBLE | VULKRON_MEMORY_USAGE_CPU_COHERENT,
	VULKRON_MEMORY_USAGE_DYNAMIC_READ_ONCE = VULKRON_MEMORY_USAGE_CPU_VISIBLE | VULKRON_MEMORY_USAGE_CPU_COHERENT | VULKRON_MEMORY_USAGE_CPU_CACHED,
	VULKRON_MEMORY_USAGE_GPU_WRITE_CPU_READ = VULKRON_MEMORY_USAGE_GPU_STORAGE | VULKRON_MEMORY_USAGE_CPU_VISIBLE | VULKRON_MEMORY_USAGE_CPU_COHERENT
} VulkronMemoryUsage;

typedef enum VulkronAttachmentFlagBits {
	VULKRON_DEFAULT_ATTACHMENT = 0,
	VULKRON_UNION_ATTACHMENT
} VulkronAttachmentFlagBits;
typedef VulkronFlags VulkronAttachmentFlags;

typedef enum VulkronQueueFlagBits {
	VULKRON_QUEUE_GRAPHICS_BIT = 0x00000001,
	VULKRON_QUEUE_COMPUTE_BIT = 0x00000002,
	VULKRON_QUEUE_TRANSFER_BIT = 0x00000004
} VulkronQueueFlagBits;
typedef VulkronFlags VulkronQueueFlag;

typedef enum VulkronAllocatorFlagBits {

} VulkronAllocatorFlagBits;
typedef VulkronFlags VulkronAllocatorFlags;

// ---------------------------------- 
// Data Ext Structs -----------------
// ----------------------------------

typedef struct VulkronGpuFeatures {
	VulkronBool32							robustBufferAccess;
	VulkronBool32							fullDrawIndexUint32;
	VulkronBool32							imageCubeArray;
	VulkronBool32							independentBlend;
	VulkronBool32							geometryShader;
	VulkronBool32							tessellationShader;
	VulkronBool32							sampleRateShading;
	VulkronBool32							dualSrcBlend;
	VulkronBool32							logicOp;
	VulkronBool32							multiDrawIndirect;
	VulkronBool32							drawIndirectFirstInstance;
	VulkronBool32							depthClamp;
	VulkronBool32							depthBiasClamp;
	VulkronBool32							fillModeNonSolid;
	VulkronBool32							depthBounds;
	VulkronBool32							wideLines;
	VulkronBool32							largePoints;
	VulkronBool32							alphaToOne;
	VulkronBool32							multiViewport;
	VulkronBool32							samplerAnisotropy;
	VulkronBool32							textureCompressionETC2;
	VulkronBool32							textureCompressionASTC_LDR;
	VulkronBool32							textureCompressionBC;
	VulkronBool32							occlusionQueryPrecise;
	VulkronBool32							pipelineStatisticsQuery;
	VulkronBool32							vertexPipelineStoresAndAtomics;
	VulkronBool32							fragmentStoresAndAtomics;
	VulkronBool32							shaderTessellationAndGeometryPointSize;
	VulkronBool32							shaderImageGatherExtended;
	VulkronBool32							shaderStorageImageExtendedFormats;
	VulkronBool32							shaderStorageImageMultisample;
	VulkronBool32							shaderStorageImageReadWithoutFormat;
	VulkronBool32							shaderStorageImageWriteWithoutFormat;
	VulkronBool32							shaderUniformBufferArrayDynamicIndexing;
	VulkronBool32							shaderSampledImageArrayDynamicIndexing;
	VulkronBool32							shaderStorageBufferArrayDynamicIndexing;
	VulkronBool32							shaderStorageImageArrayDynamicIndexing;
	VulkronBool32							shaderClipDistance;
	VulkronBool32							shaderCullDistance;
	VulkronBool32							shaderFloat64;
	VulkronBool32							shaderInt64;
	VulkronBool32							shaderInt16;
	VulkronBool32							shaderResourceResidency;
	VulkronBool32							shaderResourceMinLod;
	VulkronBool32							variableMultisampleRate;
	VulkronBool32							inheritedQueries;
} VulkronGpuFeatures;

typedef struct VulkronGraphicsPipeline {
	VkPipelineShaderStageCreateInfo*		pShaderStage;
	uint32_t								shaderStageCount;
	VkPipelineVertexInputStateCreateInfo	pVertexInputState;
	VkPipelineInputAssemblyStateCreateInfo	pInputAssemblyState;
	VkPipelineTessellationStateCreateInfo	pTessellationState;
	VkPipelineRasterizationStateCreateInfo	pRasterizationState;
	VkPipelineMultisampleStateCreateInfo	pMultisampleState;
	VkPipelineDepthStencilStateCreateInfo	pDepthStencilState;
	VkPipelineColorBlendStateCreateInfo		pColorBlendState;
	VkPipelineDynamicStateCreateInfo		pDynamicState;
	VkPipelineLayoutCreateInfo				pPipelineLayoutInfo;
} VulkronGraphicsPipeline;

typedef struct VulkronVertex {
	glm::vec3								position;
	glm::vec3								color;
	glm::vec2								texture;
} VulkronVertex;

typedef struct VulkronVertexDescriptions {
	VkVertexInputBindingDescription*		pBindings;
	uint32_t								bindingsCount;
	VkVertexInputAttributeDescription		pAttributes;
	uint32_t								attributesCount;
} VulkronVertexDescriptions;

typedef struct VulkronBaseObject {
	VkRenderPass*							pRenderPass			= nullptr;				// Renderpass that the object uses
	VkPipeline*								pPipeline			= nullptr;				// Pipeline that the object uses
	VkPipelineLayout*						pPipelineLayout		= nullptr;				// Pipeline Layout that the object uses
	uint32_t								instances			= 1;					// amount of objects of this type
	size_t									objectId			= 0;					// hashed Id
	std::string								objectName			= "Object";
	glm::mat4								model				= {};
	glm::vec3								position			= { 0.0f, 0.0f, 0.0f };
	glm::vec3								rotation			= { 0.0f, 0.0f, 0.0f };
	glm::vec3								color				= { 1.0f, 1.0f, 1.0f };
	glm::vec2								texture				= {};
	float									scale				= 1.0f;					// meters
	bool									isVisible			= true;
	bool									castShadow			= false;
	bool									receiveShadow		= false;
	bool									frustumCulling		= false;
	bool									isStatic			= true;
	VulkronBaseObject*						parent				= nullptr;
	VulkronBaseObject*						child				= nullptr;
} VulkronBaseObject;

typedef struct VulkronGraphicsCommands {
	std::vector<VulkronBaseObject>			staticObjectlist;
	std::vector<VulkronBaseObject>			dynamicObjectsList;
} VulkronGraphicsCommands;

// ---------------------------------- 
// Init Data Structs ----------------
// ----------------------------------

typedef struct VulkronInstanceCreateInfo {
	VkInstance*								pInstance;
	GLFWwindow*								pWindow;
} VulkronInstanceCreateInfo;

typedef struct VulkronDeviceCreateInfo {
	bool									isUsingSwapchain;
	VulkronQueueFlag						graphicsQueueFlag;
	uint32_t								graphicsQueueCount;
	VulkronQueueFlag						computeQueueFlag;
	uint32_t								computeQueueCount;
	VulkronQueueFlag						transferQueueFlag;
	uint32_t								transferQueueCount;
	VulkronGpuFeatures						gpuEnabledFeatures;
	VkPhysicalDevice*						pGpu;
	uint32_t								gpuCount;
} VulkronDeviceCreateInfo;

typedef struct VulkronSwapchainCreateInfo {
	uint32_t*								width;
	uint32_t*								height;
	bool									vsync;
} VulkronSwapchainCreateInfo;

typedef struct VulkronGraphicsPipelineCreateInfo {
	VulkronAttachmentFlags					flag;
	VkRenderPass*							pRenderPass;
	VkPipeline*								pPipeline;
	VkPipelineLayout*						pPipelineLayout;
	VulkronGraphicsPipeline*				pPipelineData;
} VulkronGraphicsPipelineCreateInfo;

void vulkronDrawFrame();

VulkronResult vulkronCreateInstance(VulkronInstanceCreateInfo* info);
VulkronResult vulkronCreateDevice(VulkronDeviceCreateInfo* info);
VulkronResult vulkronCreateSwapchain(VulkronSwapchainCreateInfo* info);
VulkronResult vulkronCreateGraphicsPipeline(VulkronGraphicsPipelineCreateInfo* info);
VulkronResult vulkronCreateRendererCommandBuffers(VulkronGraphicsCommands* info);
VulkronResult vulkronShutdown();

std::vector<VkPhysicalDevice> vulkronGetGpuDevicesList();
#if defined _DEBUG || defined VULKRON_ENGINE_DEBUGGING
void vulkronGpuProperties();
void vulkronGpuFeatures();
void vulkronGpuSurfaceCapabilities();
void vulkronGpuSurfaceFormats();
void vulkronGpuSurfacePresentModes();
void vulkronGpuLimits();
void vulkronGpuQueueFamilyProperties();
void vulkronGpuMemoryProperties(bool showAllProperties);
#endif // _DEBUG || VULKRON_ENGINE_DEBUGGING

VkPipelineVertexInputStateCreateInfo vulkronVertexInputState(
	uint32_t									vertexBindingDescriptionCount,
	const VkVertexInputBindingDescription*		pVertexBindingDescriptions,
	uint32_t									vertexAttributeDescriptionCount,
	const VkVertexInputAttributeDescription*	pVertexAttributeDescriptions);

VkPipelineInputAssemblyStateCreateInfo vulkronInputAssemblyState(
	VkPrimitiveTopology							topology,
	VulkronBool32								primitiveRestartEnable);

VkPipelineTessellationStateCreateInfo vulkronTessellationState(
	uint32_t									patchControlPoints);

VkPipelineRasterizationStateCreateInfo vulkronRasterizationState(
	VulkronBool32								depthClampEnable,
	VulkronBool32								rasterizerDiscardEnable,
	VkPolygonMode								polygonMode,
	VkCullModeFlags								cullMode,
	VkFrontFace									frontFace,
	VulkronBool32								depthBiasEnable,
	float										lineWidth,
	float										depthBiasConstantFactor	= 0.0f,
	float										depthBiasClamp			= 0.0f,
	float										depthBiasSlopeFactor	= 0.0f);

VkPipelineMultisampleStateCreateInfo vulkronMultisampleState(
	VkSampleCountFlagBits						rasterizationSamples,
	VulkronBool32								sampleShadingEnable,
	float										minSampleShading		= 1.0f,
	const VkSampleMask*							pSampleMask				= nullptr,
	VulkronBool32								alphaToCoverageEnable	= VULKRON_FALSE,
	VulkronBool32								alphaToOneEnable		= VULKRON_FALSE);

VkPipelineDepthStencilStateCreateInfo vulkronDepthStencilState(
	VulkronBool32								depthTestEnable,
	VulkronBool32								depthWriteEnable,
	VkCompareOp									depthCompareOp,
	VulkronBool32								depthBoundsTestEnable,
	VulkronBool32								stencilTestEnable,
	VkStencilOpState							front,
	VkStencilOpState							back,
	float										minDepthBounds,
	float										maxDepthBounds);

VkPipelineColorBlendAttachmentState vulkronColorBlendAttchementState(
	VulkronBool32								blendEnable				= VULKRON_FALSE,
	VkBlendFactor								srcColorBlendFactor		= VK_BLEND_FACTOR_ZERO,
	VkBlendFactor								dstColorBlendFactor		= VK_BLEND_FACTOR_ZERO,
	VkBlendOp									colorBlendOp			= VK_BLEND_OP_ADD,
	VkBlendFactor								srcAlphaBlendFactor		= VK_BLEND_FACTOR_ZERO,
	VkBlendFactor								dstAlphaBlendFactor		= VK_BLEND_FACTOR_ZERO,
	VkBlendOp									alphaBlendOp			= VK_BLEND_OP_ADD,
	VkColorComponentFlags						colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

VkPipelineColorBlendStateCreateInfo vulkronColorBlendState(
	VkPipelineColorBlendAttachmentState*		pColorAttachments,
	uint32_t									attachmentsCount,
	VulkronBool32								logicOpEnable,
	float										blendConstants[4],
	VkLogicOp									logicOp					= VK_LOGIC_OP_COPY);

VkPipelineDynamicStateCreateInfo vulkronDynamicState(
	uint32_t									dynamicStateCount,
	const VkDynamicState*						pDynamicStates);

VkPipelineLayoutCreateInfo vulkronPipelineLayoutInfo(
	uint32_t									setLayoutCount			= 0,
	VkDescriptorSetLayout*						pSetLayouts				= nullptr,
	uint32_t									pushConstantRangeCount	= 0,
	VkPushConstantRange*						pPushConstantRanges		= nullptr);

VkPipelineShaderStageCreateInfo vulkronCreatePipelineShaderStage(
	std::string									shaderPath, 
	VkShaderStageFlagBits						stage);

#endif // !VULKRON_CORE