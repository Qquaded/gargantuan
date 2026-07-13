#pragma once

#include "vulkan/vulkan.h"

namespace VulkanImages
{

void transitionImage(VkCommandBuffer commands, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

}