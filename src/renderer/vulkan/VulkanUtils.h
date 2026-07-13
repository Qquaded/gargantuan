#pragma once

#include "../../VkBootstrap.h"

#include "vulkan/vk_enum_string_helper.h"

namespace VulkanUtils
{

inline void assertVkResult(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(string_VkResult(result));
    }
};

template<typename T>
inline T assertVkbResult(vkb::Result<T> result, const char* baseMessage)
{
    if (result)
    {
        return result.value();
    }

    std::string errorMessage(baseMessage);
    errorMessage.append(": ");
    errorMessage.append(result.error().message());

    auto failureReasons = result.detailed_failure_reasons();
    if (failureReasons.size() != 0)
    {
        for (const auto& reason : failureReasons)
        {
            errorMessage.append("\n\t");
            errorMessage.append(reason.c_str());
        }
    }

    throw std::runtime_error(errorMessage);
};

VkCommandPoolCreateInfo createCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
VkCommandBufferAllocateInfo createCommandPoolAllocateInfo(
    VkCommandPool commandPool,
    uint32_t commandBufferCount = 1,
    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
);

VkFenceCreateInfo createFenceCreateInfo(VkFenceCreateFlags flags = 0);
VkSemaphoreCreateInfo createSemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);
VkCommandBufferBeginInfo createCommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
VkImageSubresourceRange createImageSubresourceRange(VkImageAspectFlags aspectMask);

VkSemaphoreSubmitInfo createSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
VkCommandBufferSubmitInfo createCommandBufferSubmitINfo(VkCommandBuffer cmd);

} // namespace VulkanUtils