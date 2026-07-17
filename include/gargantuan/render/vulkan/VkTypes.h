#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

struct VkAllocatedImage
{
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
};
