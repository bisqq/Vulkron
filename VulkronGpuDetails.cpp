#include "VulkronInternal.h"

static void getHeapTypes(VkMemoryType type, std::string& heapTypes);
static std::vector<VkPhysicalDevice> getPhysicalDevices();

//-------------------------------------------------------------------------------------
// SECTION [GPU DETAILS] --------------------------------------------------------------
//-------------------------------------------------------------------------------------

std::vector<VkPhysicalDevice> vulkronGetGpuDevicesList() {
    return getPhysicalDevices();
}

void vulkronGpuProperties() {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceProperties gpuProperties;

    LOG("");
    LOG("");
    LOG("   GPU Properties");
    LOG("   --------------");

    for (const VkPhysicalDevice& gpu : physicalDevices) {
        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
        uint32_t apiVer = gpuProperties.apiVersion;

        if (gpuProperties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

            LOG("   Name:                    " << gpuProperties.deviceName);
            LOG("");
            LOG("   API Version:             " << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer));
            LOG("   Driver Version:          " << gpuProperties.driverVersion);
            LOG("   Vendor ID:               " << gpuProperties.vendorID);
            LOG("   Device ID:               " << gpuProperties.deviceID);
            LOG("   Device Type:             " << gpuProperties.deviceType);
            LOG("   discreteQueuePriorities: " << gpuProperties.limits.discreteQueuePriorities);
            LOG("");
            LOG("");

        }
        else if (gpuProperties.deviceType & VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            LOG("   Name:                    " << gpuProperties.deviceName);
            LOG("");
            LOG("   API Version:             " << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer));
            LOG("   Driver Version:          " << gpuProperties.driverVersion);
            LOG("   Vendor ID:               " << gpuProperties.vendorID);
            LOG("   Device ID:               " << gpuProperties.deviceID);
            LOG("   Device Type:             " << gpuProperties.deviceType);
            LOG("   discreteQueuePriorities: " << gpuProperties.limits.discreteQueuePriorities);
            LOG("");
            LOG("");

        }
        else {
            throw std::runtime_error("No Gpu Detected");
        }

    }
}

void vulkronGpuFeatures() {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceFeatures gpuFeatures;
    VkPhysicalDeviceProperties gpuProperties;

    LOG("   GPU Features");
    LOG("   --------------");

    for (const VkPhysicalDevice& gpu : physicalDevices) {
        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
        vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);

        LOG("   Name:                                           " << gpuProperties.deviceName);
        LOG("");
        LOG("   Robust Buffer Access:                           " << (gpuFeatures.robustBufferAccess ? "True" : "False"));
        LOG("   Full Draw Index Uint32:                         " << (gpuFeatures.fullDrawIndexUint32 ? "True" : "False"));
        LOG("   Image Cube Array:                               " << (gpuFeatures.imageCubeArray ? "True" : "False"));
        LOG("   Independent Blend:                              " << (gpuFeatures.independentBlend ? "True" : "False"));
        LOG("   Geometry Shader:                                " << (gpuFeatures.geometryShader ? "True" : "False"));
        LOG("   Tessellation Shader:                            " << (gpuFeatures.tessellationShader ? "True" : "False"));
        LOG("   Sample Rate Shading:                            " << (gpuFeatures.sampleRateShading ? "True" : "False"));
        LOG("   Dual Source Blend:                              " << (gpuFeatures.dualSrcBlend ? "True" : "False"));
        LOG("   Logic Operations:                               " << (gpuFeatures.logicOp ? "True" : "False"));
        LOG("   Multi Draw Indirect:                            " << (gpuFeatures.multiDrawIndirect ? "True" : "False"));
        LOG("   Draw Indirect First Instances:                  " << (gpuFeatures.drawIndirectFirstInstance ? "True" : "False"));
        LOG("   Depth Clamp :                                   " << (gpuFeatures.depthClamp ? "True" : "False"));
        LOG("   Depth Bias Clamp:                               " << (gpuFeatures.depthBiasClamp ? "True" : "False"));
        LOG("   Fill Mode Non Solid:                            " << (gpuFeatures.fillModeNonSolid ? "True" : "False"));
        LOG("   Depth Bounds:                                   " << (gpuFeatures.depthBounds ? "True" : "False"));
        LOG("   Wide Lines  :                                   " << (gpuFeatures.wideLines ? "True" : "False"));
        LOG("   Large Points:                                   " << (gpuFeatures.largePoints ? "True" : "False"));
        LOG("   Alpha To One:                                   " << (gpuFeatures.alphaToOne ? "True" : "False"));
        LOG("   Multi Viewport:                                 " << (gpuFeatures.multiViewport ? "True" : "False"));
        LOG("   Sampler Anisotropy:                             " << (gpuFeatures.samplerAnisotropy ? "True" : "False"));
        LOG("   Texture Compression ETC2:                       " << (gpuFeatures.textureCompressionETC2 ? "True" : "False"));
        LOG("   Texture Compression ASTC_LDR:                   " << (gpuFeatures.textureCompressionASTC_LDR ? "True" : "False"));
        LOG("   Texture Compression BC:                         " << (gpuFeatures.textureCompressionBC ? "True" : "False"));
        LOG("   Occlusion Query Precise:                        " << (gpuFeatures.occlusionQueryPrecise ? "True" : "False"));
        LOG("   Pipeline Statistics Query:                      " << (gpuFeatures.pipelineStatisticsQuery ? "True" : "False"));
        LOG("   Vertex Pipeline Stores And Atomics:             " << (gpuFeatures.vertexPipelineStoresAndAtomics ? "True" : "False"));
        LOG("   Fragment Stores And Atomics:                    " << (gpuFeatures.fragmentStoresAndAtomics ? "True" : "False"));
        LOG("   Shader Tessellation And Geometry Point Size:    " << (gpuFeatures.shaderTessellationAndGeometryPointSize ? "True" : "False"));
        LOG("   Shader Image Gather Extended:                   " << (gpuFeatures.shaderImageGatherExtended ? "True" : "False"));
        LOG("   Shader Storage Image Extended Formats:          " << (gpuFeatures.shaderStorageImageExtendedFormats ? "True" : "False"));
        LOG("   Shader Storage Image Multisample:               " << (gpuFeatures.shaderStorageImageMultisample ? "True" : "False"));
        LOG("   Shader Storage Image Read Without Format:       " << (gpuFeatures.shaderStorageImageReadWithoutFormat ? "True" : "False"));
        LOG("   Shader Storage Image Write Without Format:      " << (gpuFeatures.shaderStorageImageWriteWithoutFormat ? "True" : "False"));
        LOG("   Shader Uniform Buffer Array Dynamic Indexing:   " << (gpuFeatures.shaderUniformBufferArrayDynamicIndexing ? "True" : "False"));
        LOG("   Shader Sampled Image Array Dynamic Indexing:    " << (gpuFeatures.shaderSampledImageArrayDynamicIndexing ? "True" : "False"));
        LOG("   Shader Storage Buffer Array Dynamic Indexing:   " << (gpuFeatures.shaderStorageBufferArrayDynamicIndexing ? "True" : "False"));
        LOG("   Shader Storage Image Array Dynamic Indexing:    " << (gpuFeatures.shaderStorageImageArrayDynamicIndexing ? "True" : "False"));
        LOG("   Shader Clip Distance:                           " << (gpuFeatures.shaderClipDistance ? "True" : "False"));
        LOG("   Shader Cull Distance:                           " << (gpuFeatures.shaderCullDistance ? "True" : "False"));
        LOG("   Shader Float 64:                                " << (gpuFeatures.shaderFloat64 ? "True" : "False"));
        LOG("   Shader Int 64:                                  " << (gpuFeatures.shaderInt64 ? "True" : "False"));
        LOG("   Shader Int 16:                                  " << (gpuFeatures.shaderInt16 ? "True" : "False"));
        LOG("   Shader Resource Residency:                      " << (gpuFeatures.shaderResourceResidency ? "True" : "False"));
        LOG("   Shader Resource Min Lod:                        " << (gpuFeatures.shaderResourceMinLod ? "True" : "False"));
        LOG("   Variable Multisample Rate:                      " << (gpuFeatures.variableMultisampleRate ? "True" : "False"));
        LOG("   Inherited Queries:                              " << (gpuFeatures.inheritedQueries ? "True" : "False"));
        LOG("");
        LOG("");
    }
}

void vulkronGpuSurfaceCapabilities() {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceProperties gpuProperties;
    VkSurfaceCapabilitiesKHR surfaceCapabilites;

    LOG("   Surface Capabilites");
    LOG("   -------------------");

    for (const VkPhysicalDevice& gpu : physicalDevices) {
        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, instanceInternal->surface, &surfaceCapabilites);

        LOG("   Name:                      " << gpuProperties.deviceName);
        LOG("");
        LOG("   MinImageCount:             " << surfaceCapabilites.minImageCount);
        LOG("   MaxImageCount:             " << surfaceCapabilites.maxImageCount);
        LOG("   CurrentExtent:             " << surfaceCapabilites.currentExtent.width << "/" << surfaceCapabilites.currentExtent.height);
        LOG("   MinImageExtent:            " << surfaceCapabilites.minImageExtent.width << "/" << surfaceCapabilites.minImageExtent.height);
        LOG("   MaxImageExtent:            " << surfaceCapabilites.maxImageExtent.width << "/" << surfaceCapabilites.maxImageExtent.height);
        LOG("   MaxImageArrayLayers:       " << surfaceCapabilites.maxImageArrayLayers);
        LOG("   SupportedTransforms:       " << surfaceCapabilites.supportedTransforms);
        LOG("   CurrentTransform:          " << surfaceCapabilites.currentTransform);
        LOG("   SupportedCompositeAlpha:   " << surfaceCapabilites.supportedCompositeAlpha);
        LOG("   SupportedUsageFlags:       " << surfaceCapabilites.supportedUsageFlags);
        LOG("");
        LOG("");
    }
}

void vulkronGpuSurfaceFormats() {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceProperties gpuProperties;

    for (const VkPhysicalDevice& gpu : physicalDevices) {
        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

        uint32_t amounfOfFormats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, instanceInternal->surface, &amounfOfFormats, nullptr);

        VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[amounfOfFormats];
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, instanceInternal->surface, &amounfOfFormats, surfaceFormats);

        LOG("   GPU Surface Formats");
        LOG("   -------------------");
        LOG("   Name:              " << gpuProperties.deviceName);
        LOG("");
        LOG("   Amount of Formats: " << amounfOfFormats);

        for (int i = 0; i < amounfOfFormats; i++) {
            LOG("   Format:            " << surfaceFormats[i].format);
        }

        LOG("");
        LOG("");

        delete[] surfaceFormats;
    }
}

void vulkronGpuSurfacePresentModes() {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceProperties gpuProperties;

    LOG("   GPU Surface Present Modes");
    LOG("   -------------------------");

    for (const VkPhysicalDevice& gpu : physicalDevices) {
        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

        uint32_t amountOfPresentationsModes = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, instanceInternal->surface, &amountOfPresentationsModes, nullptr);

        VkPresentModeKHR* presentModes = new VkPresentModeKHR[amountOfPresentationsModes];
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, instanceInternal->surface, &amountOfPresentationsModes, presentModes);

        LOG("");
        LOG("   Name:                          " << gpuProperties.deviceName);
        LOG("");
        LOG("   Amount of Presentation Modes:  " << amountOfPresentationsModes);

        for (int i = 0; i < amountOfPresentationsModes; i++) {
            LOG("   Supported presentation mode:   " << presentModes[i]);
        }

        delete[] presentModes;
    }

    LOG("");
    LOG("");
}

void vulkronGpuLimits() {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceProperties gpuProperties;

    LOG("   GPU Limites");
    LOG("   -----------");

    for (const VkPhysicalDevice& gpu : physicalDevices) {
        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
        LOG("   Name:                                                  " << gpuProperties.deviceName);
        LOG("");
        LOG("   Max Image Dimension 1D:                                " << gpuProperties.limits.maxImageDimension1D);
        LOG("   Max Image Dimension 2D:                                " << gpuProperties.limits.maxImageDimension2D);
        LOG("   Max Image Dimension 3D:                                " << gpuProperties.limits.maxImageDimension3D);
        LOG("   Max Image Dimension Cube:                              " << gpuProperties.limits.maxImageDimensionCube);
        LOG("   Max Image Array Layers:                                " << gpuProperties.limits.maxImageArrayLayers);
        LOG("   Max Texel Buffer Elements:                             " << gpuProperties.limits.maxTexelBufferElements);
        LOG("   Max Uniform Buffer Range:                              " << gpuProperties.limits.maxUniformBufferRange);
        LOG("   Max Storage Buffer Range:                              " << gpuProperties.limits.maxStorageBufferRange);
        LOG("   Max Push Constants Size:                               " << gpuProperties.limits.maxPushConstantsSize);
        LOG("   Max Memory Allocation Count:                           " << gpuProperties.limits.maxMemoryAllocationCount);
        LOG("   Max Sampler Allocation Count:                          " << gpuProperties.limits.maxSamplerAllocationCount);
        LOG("   Buffer Image Granularity:                              " << gpuProperties.limits.bufferImageGranularity);
        LOG("   Max Bound DescriptorSets:                              " << gpuProperties.limits.maxBoundDescriptorSets);
        LOG("   Max PerStage Descriptor Samplers:                      " << gpuProperties.limits.maxPerStageDescriptorSamplers);
        LOG("   Max PerStage Descriptor Uniform Buffers:               " << gpuProperties.limits.maxPerStageDescriptorUniformBuffers);
        LOG("   Max PerStage Descriptor Storage Buffers:               " << gpuProperties.limits.maxPerStageDescriptorStorageBuffers);
        LOG("   Max PerStage Descriptor Sampled Images:                " << gpuProperties.limits.maxPerStageDescriptorSampledImages);
        LOG("   Max PerStage Descriptor Storage Images:                " << gpuProperties.limits.maxPerStageDescriptorStorageImages);
        LOG("   Max PerStage Descriptor Input Attachments:             " << gpuProperties.limits.maxPerStageDescriptorInputAttachments);
        LOG("   Max PerStage Resources:                                " << gpuProperties.limits.maxPerStageResources);
        LOG("   Max Descriptor Set Samplers:                           " << gpuProperties.limits.maxDescriptorSetSamplers);
        LOG("   Max Descriptor Set Uniform Buffers:                    " << gpuProperties.limits.maxDescriptorSetUniformBuffers);
        LOG("   Max Descriptor Set Uniform Buffers Dynamic:            " << gpuProperties.limits.maxDescriptorSetUniformBuffersDynamic);
        LOG("   Max Descriptor Set Storage Buffers:                    " << gpuProperties.limits.maxDescriptorSetStorageBuffers);
        LOG("   Max Descriptor Set Storage Buffers Dynamic:            " << gpuProperties.limits.maxDescriptorSetStorageBuffersDynamic);
        LOG("   Max Descriptor Set Sampled Images:                     " << gpuProperties.limits.maxDescriptorSetSampledImages);
        LOG("   Max Descriptor Set Storage Images:                     " << gpuProperties.limits.maxDescriptorSetStorageImages);
        LOG("   Max Descriptor Set Input Attachments:                  " << gpuProperties.limits.maxDescriptorSetInputAttachments);
        LOG("   Max Vertex Input Attributes:                           " << gpuProperties.limits.maxVertexInputAttributes);
        LOG("   Max Vertex Input Bindings:                             " << gpuProperties.limits.maxVertexInputBindings);
        LOG("   Max Vertex Input Attribute Offset:                     " << gpuProperties.limits.maxVertexInputAttributeOffset);
        LOG("   Max Vertex Input Binding Stride:                       " << gpuProperties.limits.maxVertexInputBindingStride);
        LOG("   Max Vertex Output Components:                          " << gpuProperties.limits.maxVertexOutputComponents);
        LOG("   Max Tessellation GenerationLevel:                      " << gpuProperties.limits.maxTessellationGenerationLevel);
        LOG("   Max Tessellation PatchSize:                            " << gpuProperties.limits.maxTessellationPatchSize);
        LOG("   Max Tessellation Control Per Vertex Input Components:  " << gpuProperties.limits.maxTessellationControlPerVertexInputComponents);
        LOG("   Max Tessellation ControlPer Vertex Output Components:  " << gpuProperties.limits.maxTessellationControlPerVertexOutputComponents);
        LOG("   Max Tessellation ControlPer Patch Output Components:   " << gpuProperties.limits.maxTessellationControlPerPatchOutputComponents);
        LOG("   Max Tessellation Control Total Output Components:      " << gpuProperties.limits.maxTessellationControlTotalOutputComponents);
        LOG("   Max Tessellation Evaluation Input Components:          " << gpuProperties.limits.maxTessellationEvaluationInputComponents);
        LOG("   Max Tessellation Evaluation Output Components:         " << gpuProperties.limits.maxTessellationEvaluationOutputComponents);
        LOG("   Max Geometry Shader Invocations:                       " << gpuProperties.limits.maxGeometryShaderInvocations);
        LOG("   Max Geometry Input Components:                         " << gpuProperties.limits.maxGeometryInputComponents);
        LOG("   Max Geometry Output Components:                        " << gpuProperties.limits.maxGeometryOutputComponents);
        LOG("   Max Geometry Output Vertices:                          " << gpuProperties.limits.maxGeometryOutputVertices);
        LOG("   Max Geometry Total Output Components:                  " << gpuProperties.limits.maxGeometryTotalOutputComponents);
        LOG("   Max Fragment Input Components:                         " << gpuProperties.limits.maxFragmentInputComponents);
        LOG("   Max Fragment Output Attachments:                       " << gpuProperties.limits.maxFragmentOutputAttachments);
        LOG("   Max Fragment Dual Source Attachments:                  " << gpuProperties.limits.maxFragmentDualSrcAttachments);
        LOG("   Max Fragment Combined Output Resources:                " << gpuProperties.limits.maxFragmentCombinedOutputResources);
        LOG("   Max Compute Shared Memory Size:                        " << gpuProperties.limits.maxComputeSharedMemorySize);
        LOG("   Max Compute Work Group Count[3]:                       " << gpuProperties.limits.maxComputeWorkGroupCount[3]);
        LOG("   Max Compute Work Group Invocations:                    " << gpuProperties.limits.maxComputeWorkGroupInvocations);
        LOG("   Max Compute Work Group Size[3]:                        " << gpuProperties.limits.maxComputeWorkGroupSize[3]);
        LOG("   SubPixel Precision Bits:                               " << gpuProperties.limits.subPixelPrecisionBits);
        LOG("   SubTexel Precision Bits:                               " << gpuProperties.limits.subTexelPrecisionBits);
        LOG("   Mipmap Precision Bits:                                 " << gpuProperties.limits.mipmapPrecisionBits);
        LOG("   Max Draw Indexed Index Value:                          " << gpuProperties.limits.maxDrawIndexedIndexValue);
        LOG("   Max Draw Indirect Count:                               " << gpuProperties.limits.maxDrawIndirectCount);
        LOG("   Max Sampler Lod Bias:                                  " << gpuProperties.limits.maxSamplerLodBias);
        LOG("   Max Sampler Anisotropy:                                " << gpuProperties.limits.maxSamplerAnisotropy);
        LOG("   Max Viewports:                                         " << gpuProperties.limits.maxViewports);
        LOG("   Max Viewport Dimensions[2]:                            " << gpuProperties.limits.maxViewportDimensions[2]);
        LOG("   Viewport Bounds Range[2]:                              " << gpuProperties.limits.viewportBoundsRange[2]);
        LOG("   Viewport SubPixel Bits:                                " << gpuProperties.limits.viewportSubPixelBits);
        LOG("   MinMemory Map Alignment:                               " << gpuProperties.limits.minMemoryMapAlignment);
        LOG("   MinTexel Buffer Offset Alignment:                      " << gpuProperties.limits.minTexelBufferOffsetAlignment);
        LOG("   Min Uniform Buffer Offset Alignment:                   " << gpuProperties.limits.minUniformBufferOffsetAlignment);
        LOG("   Min Storage Buffer Offset Alignment:                   " << gpuProperties.limits.minStorageBufferOffsetAlignment);
        LOG("   Min Texel Offset:                                      " << gpuProperties.limits.minTexelOffset);
        LOG("   Max Texel Offset:                                      " << gpuProperties.limits.maxTexelOffset);
        LOG("   Min Texel Gather Offset:                               " << gpuProperties.limits.minTexelGatherOffset);
        LOG("   Max Texel Gather Offset:                               " << gpuProperties.limits.maxTexelGatherOffset);
        LOG("   Min Interpolation Offset:                              " << gpuProperties.limits.minInterpolationOffset);
        LOG("   Max Interpolation Offset:                              " << gpuProperties.limits.maxInterpolationOffset);
        LOG("   SubPixel Interpolation Offset Bits:                    " << gpuProperties.limits.subPixelInterpolationOffsetBits);
        LOG("   Max Frame buffer Width:                                " << gpuProperties.limits.maxFramebufferWidth);
        LOG("   Max Frame buffer Height:                               " << gpuProperties.limits.maxFramebufferHeight);
        LOG("   Max Frame buffer Layers:                               " << gpuProperties.limits.maxFramebufferLayers);
        LOG("   Frame buffer Color Sample Counts:                      " << gpuProperties.limits.framebufferColorSampleCounts);
        LOG("   Frame buffer Depth Sample Counts:                      " << gpuProperties.limits.framebufferDepthSampleCounts);
        LOG("   Frame buffer Stencil Sampl eCounts:                    " << gpuProperties.limits.framebufferStencilSampleCounts);
        LOG("   Frame buffer No Attachments Sample Counts:             " << gpuProperties.limits.framebufferNoAttachmentsSampleCounts);
        LOG("   Max Color Attachments:                                 " << gpuProperties.limits.maxColorAttachments);
        LOG("   Sampled Image Color Sample Counts:                     " << gpuProperties.limits.sampledImageColorSampleCounts);
        LOG("   Sampled Image Integer Sample Counts:                   " << gpuProperties.limits.sampledImageIntegerSampleCounts);
        LOG("   Sampled Image Depth Sample Counts:                     " << gpuProperties.limits.sampledImageDepthSampleCounts);
        LOG("   Sampled Image Stencil Sample Counts:                   " << gpuProperties.limits.sampledImageStencilSampleCounts);
        LOG("   Storage Image Sample Counts:                           " << gpuProperties.limits.storageImageSampleCounts);
        LOG("   Max Sample Mask Words:                                 " << gpuProperties.limits.maxSampleMaskWords);
        LOG("   Timestamp Compute And Graphics:                        " << gpuProperties.limits.timestampComputeAndGraphics);
        LOG("   Timestamp Period:                                      " << gpuProperties.limits.timestampPeriod);
        LOG("   Max Clip Distances:                                    " << gpuProperties.limits.maxClipDistances);
        LOG("   Max Cull Distances:                                    " << gpuProperties.limits.maxCullDistances);
        LOG("   Max Combined Clip And Cull Distances:                  " << gpuProperties.limits.maxCombinedClipAndCullDistances);
        LOG("   Discrete Queue Priorities:                             " << gpuProperties.limits.discreteQueuePriorities);
        LOG("   Point Size Range[2]:                                   " << gpuProperties.limits.pointSizeRange[2]);
        LOG("   Line Width Range[2]:                                   " << gpuProperties.limits.lineWidthRange[2]);
        LOG("   Point Size Granularity:                                " << gpuProperties.limits.pointSizeGranularity);
        LOG("   Line Width Granularity:                                " << gpuProperties.limits.lineWidthGranularity);
        LOG("   Strict Lines:                                          " << gpuProperties.limits.strictLines);
        LOG("   Standard Sample Locations:                             " << gpuProperties.limits.standardSampleLocations);
        LOG("   Optimal BufferCopy Offset Alignment:                   " << gpuProperties.limits.optimalBufferCopyOffsetAlignment);
        LOG("   Nptimal BufferCopy Row Pitch Alignment:                " << gpuProperties.limits.optimalBufferCopyRowPitchAlignment);
        LOG("   NonCoherent Atom Size:                                 " << gpuProperties.limits.nonCoherentAtomSize);
        LOG("");
        LOG("");

    }
}

void vulkronGpuQueueFamilyProperties() {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceProperties gpuProperties;

    LOG("   GPU Queue Family Properties");
    LOG("   ---------------------------");

    for (const VkPhysicalDevice& gpu : physicalDevices) {

        uint32_t index = 0;

        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilyProperties.data());

        VulkronBool32 presentation_supported = VULKRON_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index, instanceInternal->surface, &presentation_supported);

        LOG("   Name:                               " << gpuProperties.deviceName);

        std::string queues = "";
        std::string otherQueues = "";

        for (const VkQueueFamilyProperties& queueProperties : queueFamilyProperties) {

            VkExtent3D image = queueProperties.minImageTransferGranularity;

            if (queueProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queues += "| Graphics[" + std::to_string(queueProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) + "] ";
            }

            if (queueProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                queues += "| Compute[" + std::to_string(queueProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) + "] ";
            }

            if (queueProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                queues += "| Transfer[" + std::to_string(queueProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) + "] | ";
            }

            if (queueProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
                otherQueues += "| Sparse[" + std::to_string(queueProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) + "] |";
            }

            LOG("");
            LOG("   Queue Family [" << index << "] Properties: ");
            LOG("   -------------------------------------------");
            LOG("   Queue Flags Used:                   " << queues);
            LOG("   Other Queue Flags:                  " << otherQueues);
            LOG("   Total Amount Of Queues:             " << queueProperties.queueFlags);
            LOG("   Queue Count:                        " << queueProperties.queueCount);
            LOG("   TimeStamp Valid Bits:               " << queueProperties.timestampValidBits);
            LOG("   Min Image Transfer Granularity:     " << "Width: " << image.width << " | " << "Height: " << image.height << " | " << "Depth: " << image.depth);
            LOG("   Presentation Supported:             " << (presentation_supported ? "True" : "False"));
            LOG("");

            index++;
            queues = "";
            otherQueues = "";
        }

        LOG("");
        LOG("");
    }
}

void vulkronGpuMemoryProperties(bool showAllProperties) {

    std::vector<VkPhysicalDevice> physicalDevices = getPhysicalDevices();
    VkPhysicalDeviceProperties gpuProperties;
    VkPhysicalDeviceMemoryProperties memoryProperties;

    // special number that Microsoft x64 uses for unallocated memory
    uint32_t unallocatedMemory = 0xcccccccc;

    LOG("")
    LOG("   GPU Memory Properties");
    LOG("   ---------------------");

    for (const VkPhysicalDevice& gpu : physicalDevices) {

        uint32_t unallocatedMemoryCount = 0;
        uint32_t unallocatedMemoryFlagsCount = 0;

        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
        vkGetPhysicalDeviceMemoryProperties(gpu, &memoryProperties);

        LOG("   Name:                           " << gpuProperties.deviceName);
        LOG("");
        LOG("   Total Amount of Heaps:          " << memoryProperties.memoryHeapCount);
        LOG("   Total Allocated Heap Indexes:   " << memoryProperties.memoryTypeCount);

        for (auto type : memoryProperties.memoryTypes) {
            std::string heapTypes = "";

            getHeapTypes(type, heapTypes);
            
            if (type.heapIndex == unallocatedMemory) {
                unallocatedMemoryCount++;
            }

            if (heapTypes == "") {
                unallocatedMemoryFlagsCount++;
            }
        }

        LOG("   Total Unallocated Heap Indexes: " << unallocatedMemoryCount);
        LOG("   Total Empty Heaps Indexes:      " << unallocatedMemoryFlagsCount);
        LOG("   ---------------------");
        LOG("");

        for (auto type : memoryProperties.memoryTypes) {
            std::string heapTypes = "";

            getHeapTypes(type, heapTypes);

            if (showAllProperties) {
                LOG("    Heap Index [" << type.heapIndex << "]: " << heapTypes);
            } 
            else if (heapTypes != "" && type.heapIndex != unallocatedMemory) {
                LOG("    Heap Index [" << type.heapIndex << "]: " << heapTypes);
            }

        }

        LOG("");
        LOG("");
    }

    LOG("");
    LOG("");
}

static void getHeapTypes(VkMemoryType type, std::string& heapTypes) {
    if (type.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        heapTypes += "| Device Local ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        heapTypes += "| Host Visible ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        heapTypes += "| Host Coherent ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
        heapTypes += "| Host Cached ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) {
        heapTypes += "| Lazily Allocated ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT) {
        heapTypes += "| Protected ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD) {
        heapTypes += "| Device Coherent AMD ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD) {
        heapTypes += "| Device Uncached AMD ";
    }
    if (type.propertyFlags & VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV) {
        heapTypes += "| RDMA Capable Nivida ";
    }
}

static std::vector<VkPhysicalDevice> getPhysicalDevices() {
    uint32_t numberOfGpus = 0;
    vkEnumeratePhysicalDevices(*instance->pInstance, &numberOfGpus, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(numberOfGpus);
    vkEnumeratePhysicalDevices(*instance->pInstance, &numberOfGpus, physicalDevices.data());

    return physicalDevices;
}