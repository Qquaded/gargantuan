#include "gargantuan/render/vulkan/VkStruct.h"

namespace VkStruct
{

VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = flags;
    info.queueFamilyIndex = queueFamilyIndex;
    return info;
}

VkCommandBufferAllocateInfo CommandPoolAllocateInfo(VkCommandPool commandPool, uint32_t commandBufferCount, VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = commandPool;
    info.commandBufferCount = commandBufferCount;
    info.level = level;
    return info;
}

VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags)
{
    VkFenceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
};

VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
{
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
};


VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}

VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask)
{
    VkImageSubresourceRange info{};
    info.aspectMask = aspectMask;
    info.baseMipLevel = 0;
    info.levelCount = VK_REMAINING_MIP_LEVELS;
    info.baseArrayLayer = 0;
    info.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return info;
}

VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
    VkSemaphoreSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.semaphore = semaphore;
    info.stageMask = stageMask;
    info.deviceIndex = 0;
    info.value = 1;
    return info;
}

VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd)
{
    VkCommandBufferSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext = nullptr;
    info.commandBuffer = cmd;
    info.deviceMask = 0;

    return info;
}


VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
{
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;

    info.imageType = VK_IMAGE_TYPE_2D;

    info.format = format;
    info.extent = extent;

    info.mipLevels = 1;
    info.arrayLayers = 1;

    // for MSAA. we will not be using it by default, so default it to 1 sample per pixel.
    info.samples = VK_SAMPLE_COUNT_1_BIT;

    // optimal tiling, which means the image is stored on the best gpu format
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = usageFlags;

    return info;
}

VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;

    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = aspectFlags;

    return info;
}

} // namespace VkStruct