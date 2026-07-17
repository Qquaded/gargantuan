#pragma once

#include <deque>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

class VkResources
{
public:
    VkResources() = default;
    VkResources(VkDevice device, VmaAllocator allocator);
    ~VkResources();

    VkResources(const VkResources&) = delete;
    VkResources& operator=(const VkResources&) = delete;

    VkResources(VkResources&&) noexcept = default;
    VkResources& operator=(VkResources&&) noexcept = default;

    void Flush();

    VkBuffer Push(VkBuffer buffer, VmaAllocation allocation)
    {
        PushResource(ResourceType::VmaBuffer, reinterpret_cast<uintptr_t>(buffer), reinterpret_cast<uintptr_t>(allocation));
        return buffer;
    };

    VkImageView Push(VkImageView imageView)
    {
        PushResource(ResourceType::ImageView, reinterpret_cast<uintptr_t>(imageView), 0);
        return imageView;
    };

private:
    enum class ResourceType
    {
        VmaBuffer,
        ImageView
    };

    struct Resource
    {
        ResourceType type;
        uintptr_t handle;
        uintptr_t allocation;
    };

    void PushResource(ResourceType type, uintptr_t handle, uintptr_t allocation);

    VkDevice device;
    VmaAllocator allocator;
    std::deque<Resource> resources;
};