#include "gargantuan/render/vulkan/VkResources.h"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

VkResources::VkResources(VkDevice device, VmaAllocator allocator)
    : device(device)
    , allocator(allocator) {};

VkResources::~VkResources()
{
    Flush();
};

void VkResources::PushResource(VkResources::ResourceType type, uintptr_t handle, uintptr_t allocation)
{
    Resource resource{.type = type, .handle = handle, .allocation = allocation};
    resources.emplace_front(resource);
};

void VkResources::Flush()
{
    for (auto& resource : resources)
    {
        switch (resource.type)
        {
        case VkResources::ResourceType::VmaBuffer:
            vmaDestroyBuffer(allocator, reinterpret_cast<VkBuffer>(resource.handle), reinterpret_cast<VmaAllocation>(resource.allocation));
            break;
        case VkResources::ResourceType::ImageView:
            vkDestroyImageView(device, reinterpret_cast<VkImageView>(resource.handle), nullptr);
            break;
        }
    }
    resources.clear();
}