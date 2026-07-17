#pragma once

#include "vulkan/vulkan.h"

namespace VkImages
{

void TransitionImage(VkCommandBuffer commands, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

}
