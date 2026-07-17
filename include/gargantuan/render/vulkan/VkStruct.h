#pragma once

#include "vulkan/vulkan.h"

namespace VkStruct
{

VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
VkCommandBufferAllocateInfo CommandPoolAllocateInfo(
    VkCommandPool commandPool,
    uint32_t commandBufferCount = 1,
    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
);

VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = 0);
VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);
VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask);

VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd);

}; // namespace VkStruct
