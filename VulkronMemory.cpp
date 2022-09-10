#pragma once

#include "VulkronInternal.h"

static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);

static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) {

    for (uint32_t i = 0; i < deviceInternal->gpuMemoryProperties.memoryTypeCount; i++) {

        VkMemoryPropertyFlags flags = deviceInternal->gpuMemoryProperties.memoryTypes[i].propertyFlags;

        uint32_t isbitMemorytypesSuitable = typeFilter & (1 << i);

        uint32_t isMemorytypeSuitableForBuffer = (flags & propertyFlags) == propertyFlags;

        if (isbitMemorytypesSuitable && isMemorytypeSuitableForBuffer) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}