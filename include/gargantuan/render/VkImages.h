#pragma once

#include "vulkan/vulkan.h"

namespace VkImages
{

void transitionImage(VkCommandBuffer commands, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

}
