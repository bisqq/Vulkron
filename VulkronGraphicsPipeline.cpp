#include "VulkronInternal.h"

VulkronGraphicsPipelineCreateInfo*	pipeline	        = new VulkronGraphicsPipelineCreateInfo();
RenderPassInternal*		            renderPassInternal	= new RenderPassInternal();

static std::vector<VkShaderModule> shaderModuleList;

static void createRenderPass(RenderPassInternal* info, VulkronAttachmentFlags flag);
static void pipelineCache(std::string* filePath);
static void createFrameBuffers(VulkronAttachmentFlags flag);
static void createImageViews(VulkronAttachmentFlags flag, std::vector<VkImageView>* attachments);


VulkronResult vulkronCreateGraphicsPipeline(VulkronGraphicsPipelineCreateInfo* info) {
    pipeline = info;

    if (nullptr == pipeline) {
        return VULKRON_ERROR_MEMORY_ALLOCATE;
    }

    createRenderPass(pipeline->flag);
    createGraphicsPipeline();

    return VULKRON_SUCCESS;
}

void destroyPipeline() {
    delete pipeline;
    delete renderPassInternal;
}

void destroyFrameBuffer() {

}


//-------------------------------------------------------------------------------------
// SECTION [PIPELINE] -----------------------------------------------------------------
//-------------------------------------------------------------------------------------

void createRenderPass(VulkronAttachmentFlags flag) {

    RenderPassInternal renderPassInfo = {};
    uint32_t index = 0;

    renderPassInfo.flag = flag;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapchainInternal->swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    renderPassInfo.attachmentsList.push_back(colorAttachment);

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = index;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    index++;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    renderPassInfo.subpassList.push_back(subpass);

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyList.push_back(dependency);

    // Other Attachments to be added
    //if (flag == VULKRON_UNION_ATTACHMENT) {

    //}

    createRenderPass(&renderPassInfo, flag);
}

static void createRenderPass(RenderPassInternal* info, VulkronAttachmentFlags flag) {

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    renderPassInfo.attachmentCount = static_cast<uint32_t>(info->attachmentsList.size());
    renderPassInfo.pAttachments = info->attachmentsList.data();

    renderPassInfo.subpassCount = info->subpassList.size();
    renderPassInfo.pSubpasses = info->subpassList.data();

    renderPassInfo.dependencyCount = info->dependencyList.size();
    renderPassInfo.pDependencies = info->dependencyList.data();

    if (vkCreateRenderPass(deviceInternal->logicalDevice, &renderPassInfo, nullptr, pipeline->pRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    createFrameBuffers(flag);
}

VkPipelineVertexInputStateCreateInfo vulkronVertexInputState(uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription* pVertexBindingDescriptions, 
    uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription* pVertexAttributeDescriptions) {
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.pVertexBindingDescriptions = pVertexBindingDescriptions;//&bindingDescription;
    vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptionCount;//1;
    vertexInputInfo.pVertexAttributeDescriptions = pVertexAttributeDescriptions;//attributeDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;//static_cast<uint32_t>(attributeDescriptions.size());

    return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo vulkronInputAssemblyState(VkPrimitiveTopology topology, VulkronBool32 primitiveRestartEnable) {
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = nullptr;
    inputAssembly.flags = 0;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = primitiveRestartEnable;

    return inputAssembly;
}

VkPipelineTessellationStateCreateInfo vulkronTessellationState(uint32_t	patchControlPoints) {
    VkPipelineTessellationStateCreateInfo tessellation = {};
    tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellation.pNext = nullptr;
    tessellation.flags = 0;
    tessellation.patchControlPoints = patchControlPoints;

    return tessellation;
}

VkPipelineRasterizationStateCreateInfo vulkronRasterizationState(VulkronBool32 depthClampEnable, VulkronBool32 rasterizerDiscardEnable, VkPolygonMode polygonMode, VkCullModeFlags cullMode, 
    VkFrontFace frontFace, VulkronBool32 depthBiasEnable, float lineWidth, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext = nullptr;
    rasterizer.flags = 0;
    rasterizer.depthClampEnable = depthClampEnable;
    rasterizer.rasterizerDiscardEnable = rasterizerDiscardEnable;
    rasterizer.polygonMode = polygonMode;
    rasterizer.cullMode = cullMode;
    rasterizer.frontFace = frontFace;
    rasterizer.depthBiasEnable = depthBiasEnable;
    rasterizer.depthBiasConstantFactor = depthBiasConstantFactor;
    rasterizer.depthBiasClamp = depthBiasClamp;
    rasterizer.depthBiasSlopeFactor = depthBiasSlopeFactor;
    rasterizer.lineWidth = lineWidth;

    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo vulkronMultisampleState(VkSampleCountFlagBits rasterizationSamples, VulkronBool32 sampleShadingEnable, float minSampleShading,
    const VkSampleMask* pSampleMask, VulkronBool32 alphaToCoverageEnable, VulkronBool32 alphaToOneEnable) {

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext = nullptr;
    multisampling.flags = 0;
    multisampling.rasterizationSamples = rasterizationSamples;
    multisampling.sampleShadingEnable = sampleShadingEnable;
    multisampling.minSampleShading = minSampleShading;
    multisampling.pSampleMask = pSampleMask;
    multisampling.alphaToCoverageEnable = alphaToCoverageEnable;
    multisampling.alphaToOneEnable = alphaToOneEnable;

    return multisampling;
}

VkPipelineDepthStencilStateCreateInfo vulkronDepthStencilState(VulkronBool32 depthTestEnable, VulkronBool32 depthWriteEnable, VkCompareOp depthCompareOp, VulkronBool32 depthBoundsTestEnable,
    VulkronBool32 stencilTestEnable, VkStencilOpState front, VkStencilOpState back, float minDepthBounds, float maxDepthBounds) {

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.pNext = nullptr;
    depthStencil.flags = 0;
    depthStencil.depthTestEnable = depthTestEnable;
    depthStencil.depthWriteEnable = depthWriteEnable;
    depthStencil.depthCompareOp = depthCompareOp;
    depthStencil.depthBoundsTestEnable = depthBoundsTestEnable;
    depthStencil.stencilTestEnable = stencilTestEnable;
    depthStencil.front = front;
    depthStencil.back = back;
    depthStencil.minDepthBounds = minDepthBounds;
    depthStencil.maxDepthBounds = maxDepthBounds;

    return depthStencil;
}

VkPipelineColorBlendAttachmentState vulkronColorBlendAttchementState(VulkronBool32 blendEnable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, 
    VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask) {

    VkPipelineColorBlendAttachmentState colorAttachment = {};
    colorAttachment.blendEnable = blendEnable;
    colorAttachment.srcColorBlendFactor = srcColorBlendFactor;
    colorAttachment.dstColorBlendFactor = dstColorBlendFactor;
    colorAttachment.colorBlendOp = colorBlendOp;
    colorAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
    colorAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
    colorAttachment.alphaBlendOp = alphaBlendOp;
    colorAttachment.colorWriteMask = colorWriteMask;

    return colorAttachment;
}

VkPipelineColorBlendStateCreateInfo vulkronColorBlendState(VkPipelineColorBlendAttachmentState* pColorAttachments, uint32_t attachmentsCount, VulkronBool32 logicOpEnable, float blendConstants[4], VkLogicOp logicOp) {
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;
    colorBlending.flags = 0;
    colorBlending.logicOpEnable = logicOpEnable;
    colorBlending.logicOp = logicOp;
    colorBlending.attachmentCount = attachmentsCount;
    colorBlending.pAttachments = pColorAttachments;
    colorBlending.blendConstants[0] = blendConstants[0];
    colorBlending.blendConstants[1] = blendConstants[1];
    colorBlending.blendConstants[2] = blendConstants[2];
    colorBlending.blendConstants[3] = blendConstants[3];

    return colorBlending;
}

VkPipelineDynamicStateCreateInfo vulkronDynamicState(uint32_t dynamicStateCount, const VkDynamicState* pDynamicStates) {
    VkPipelineDynamicStateCreateInfo dynamic = {};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.pNext = nullptr;
    dynamic.flags = 0;
    dynamic.dynamicStateCount = dynamicStateCount;
    dynamic.pDynamicStates = pDynamicStates;

    return dynamic;
}

VkPipelineLayoutCreateInfo vulkronPipelineLayoutInfo(uint32_t setLayoutCount, VkDescriptorSetLayout* pSetLayouts, uint32_t pushConstantRangeCount, VkPushConstantRange* pPushConstantRanges) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = setLayoutCount;
    pipelineLayoutInfo.pSetLayouts = pSetLayouts;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
    pipelineLayoutInfo.pPushConstantRanges = pPushConstantRanges;

    return pipelineLayoutInfo;
}

VkPipelineShaderStageCreateInfo vulkronCreatePipelineShaderStage(std::string shaderPath, VkShaderStageFlagBits stage) {
    
    std::ifstream file(shaderPath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = buffer.size();
    shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    buffer.clear();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(deviceInternal->logicalDevice, &shaderCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    shaderModuleList.push_back(shaderModule);

    VkPipelineShaderStageCreateInfo shaderStageInfo = {};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.pNext = nullptr;
    shaderStageInfo.flags = 0;
    shaderStageInfo.stage = stage;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = "main";
    shaderStageInfo.pSpecializationInfo = nullptr;

    return shaderStageInfo;
}

void createGraphicsPipeline() {

    VulkronGraphicsPipeline graphics = *pipeline->pPipelineData;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainInternal->swapChainExtent.width;
    viewport.height = (float)swapchainInternal->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchainInternal->swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    if (vkCreatePipelineLayout(deviceInternal->logicalDevice, &graphics.pPipelineLayoutInfo, nullptr, pipeline->pPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfoCreate = {};
    pipelineInfoCreate.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfoCreate.pNext = nullptr;
    pipelineInfoCreate.flags = 0;
    pipelineInfoCreate.stageCount = graphics.shaderStageCount;
    pipelineInfoCreate.pStages = graphics.pShaderStage;
    pipelineInfoCreate.pVertexInputState = &graphics.pVertexInputState;
    pipelineInfoCreate.pInputAssemblyState = &graphics.pInputAssemblyState;
    pipelineInfoCreate.pTessellationState = &graphics.pTessellationState;
    pipelineInfoCreate.pViewportState = &viewportState;
    pipelineInfoCreate.pRasterizationState = &graphics.pRasterizationState;
    pipelineInfoCreate.pMultisampleState = &graphics.pMultisampleState;
    pipelineInfoCreate.pDepthStencilState = &graphics.pDepthStencilState;
    pipelineInfoCreate.pColorBlendState = &graphics.pColorBlendState;
    pipelineInfoCreate.pDynamicState = &graphics.pDynamicState;
    pipelineInfoCreate.layout = *pipeline->pPipelineLayout;
    pipelineInfoCreate.renderPass = *pipeline->pRenderPass;
    pipelineInfoCreate.subpass = 0;
    pipelineInfoCreate.basePipelineHandle = VULKRON_NULL_HANDLE;


    if (vkCreateGraphicsPipelines(deviceInternal->logicalDevice, VULKRON_NULL_HANDLE, 1, &pipelineInfoCreate, nullptr, pipeline->pPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    for (VkShaderModule shaderModule : shaderModuleList) {
        vkDestroyShaderModule(deviceInternal->logicalDevice, shaderModule, nullptr);
    }

}

static void pipelineCache(std::string* filePath) {

}


//-------------------------------------------------------------------------------------
//	SECTION [FRAME BUFFER] ------------------------------------------------------------
//-------------------------------------------------------------------------------------


static void createFrameBuffers(VulkronAttachmentFlags flag) {

    vkGetSwapchainImagesKHR(deviceInternal->logicalDevice, swapchainInternal->swapChain, &swapchainInternal->imageCount, nullptr);
    swapchainInternal->swapChainImagesList.resize(swapchainInternal->imageCount);
    vkGetSwapchainImagesKHR(deviceInternal->logicalDevice, swapchainInternal->swapChain, &swapchainInternal->imageCount, swapchainInternal->swapChainImagesList.data());

    swapchainInternal->bufferList.resize(swapchainInternal->imageCount);

    for (uint32_t i = 0; i < swapchainInternal->imageCount; i++) {
        VkImageViewCreateInfo defaultColorImageView = {};
        defaultColorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        defaultColorImageView.pNext = NULL;
        defaultColorImageView.format = swapchainInternal->swapChainImageFormat;
        defaultColorImageView.components = {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A
        };
        defaultColorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        defaultColorImageView.subresourceRange.baseMipLevel = 0;
        defaultColorImageView.subresourceRange.levelCount = 1;
        defaultColorImageView.subresourceRange.baseArrayLayer = 0;
        defaultColorImageView.subresourceRange.layerCount = 1;
        defaultColorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        defaultColorImageView.flags = 0;

        swapchainInternal->bufferList[i].image = swapchainInternal->swapChainImagesList[i];

        defaultColorImageView.image = swapchainInternal->bufferList[i].image;

        vkCreateImageView(deviceInternal->logicalDevice, &defaultColorImageView, nullptr, &swapchainInternal->bufferList[i].view);

        // Other Attachments to be added
        //if (flag == VULKRON_UNION_ATTACHMENT) {

        //}

        std::vector<VkImageView> attachments = {};// Render pass attachments for frame buffer
        createImageViews(flag, &attachments);
        attachments.push_back(swapchainInternal->bufferList[i].view);

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = *pipeline->pRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapchainInternal->swapChainExtent.width;
        framebufferInfo.height = swapchainInternal->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(deviceInternal->logicalDevice, &framebufferInfo, nullptr, &swapchainInternal->bufferList[i].frameBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

static void createImageViews(VulkronAttachmentFlags flag, std::vector<VkImageView>* attachments) {

}

//-------------------------------------------------------------------------------------
//	SECTION [VERTEX] ------------------------------------------------------------------
//-------------------------------------------------------------------------------------

//VkVertexInputBindingDescription vulkronGetBindingDescription() {
//    VkVertexInputBindingDescription VertexInputBindingDescription = {};
//
//    VertexInputBindingDescription.binding = 0;
//    VertexInputBindingDescription.stride = sizeof(VulkronVertex);
//    VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//    return VertexInputBindingDescription;
//}
//
//std::array<VkVertexInputAttributeDescription, 2> vulkronGetAttributeDescriptionNoTexture() {
//
//    std::array<VkVertexInputAttributeDescription, 2> VertexInputAttributeDescriptions = {};
//
//    VertexInputAttributeDescriptions[0].binding = 0;
//    VertexInputAttributeDescriptions[0].location = 0;
//    VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
//    VertexInputAttributeDescriptions[0].offset = offsetof(VulkronVertex, position);
//
//    VertexInputAttributeDescriptions[1].binding = 0;
//    VertexInputAttributeDescriptions[1].location = 1;
//    VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//    VertexInputAttributeDescriptions[1].offset = offsetof(VulkronVertex, color);
//
//
//    return VertexInputAttributeDescriptions;
//}
//
//std::array<VkVertexInputAttributeDescription, 3> vulkronGetAttributeDescription() {
//
//    std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescriptions = {};
//
//    VertexInputAttributeDescriptions[0].binding = 0;
//    VertexInputAttributeDescriptions[0].location = 0;
//    VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
//    VertexInputAttributeDescriptions[0].offset = offsetof(VulkronVertex, position);
//
//    VertexInputAttributeDescriptions[1].binding = 0;
//    VertexInputAttributeDescriptions[1].location = 1;
//    VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//    VertexInputAttributeDescriptions[1].offset = offsetof(VulkronVertex, color);
//
//    VertexInputAttributeDescriptions[2].binding = 0;
//    VertexInputAttributeDescriptions[2].location = 2;
//    VertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
//    VertexInputAttributeDescriptions[2].offset = offsetof(VulkronVertex, texture);
//
//    return VertexInputAttributeDescriptions;
//}