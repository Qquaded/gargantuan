#include "VulkanUtils.h"

#include "vulkan/vk_enum_string_helper.h"

namespace VulkanUtils
{

VkCommandPoolCreateInfo createCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = flags;
    info.queueFamilyIndex = queueFamilyIndex;
    return info;
}

VkCommandBufferAllocateInfo createCommandPoolAllocateInfo(VkCommandPool commandPool, uint32_t commandBufferCount, VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = commandPool;
    info.commandBufferCount = commandBufferCount;
    info.level = level;
    return info;
}

VkFenceCreateInfo createFenceCreateInfo(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
};

VkSemaphoreCreateInfo createSemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
};


VkCommandBufferBeginInfo createCommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;

    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}

VkImageSubresourceRange createImageSubresourceRange(VkImageAspectFlags aspectMask)
{
    VkImageSubresourceRange subImage{};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return subImage;
}

VkSemaphoreSubmitInfo createSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
    VkSemaphoreSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.semaphore = semaphore;
    submitInfo.stageMask = stageMask;
    submitInfo.deviceIndex = 0;
    submitInfo.value = 1;

    return submitInfo;
}

VkCommandBufferSubmitInfo createCommandBufferSubmitINfo(VkCommandBuffer cmd)
{
    VkCommandBufferSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext = nullptr;
    info.commandBuffer = cmd;
    info.deviceMask = 0;

    return info;
}

// VkSubmitInfo createSubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo)
// {
//     VkSubmitInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
//     info.pNext = nullptr;

//     info.waitSemaphoreCount = waitSemaphoreInfo == nullptr ? 0 : 1;
//     info.pWaitSemaphores = waitSemaphoreInfo->semaphore;

//     info.signalSemaphoreCount = signalSemaphoreInfo == nullptr ? 0 : 1;
//     info.pSignalSemaphores = signalSemaphoreInfo;

//     info.commandBufferCount = 1;
//     info.pCommandBuffers = cmd;

//     return info;
// }

} // namespace VulkanUtils