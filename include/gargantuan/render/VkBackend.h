#pragma once

#include <SDL3/SDL.h>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>

class VkBackend
{
public:
    VkBackend();

    SDL_Window* window;

    VkInstance instance;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily = 0;

    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocator allocator;

    void Destroy();

private:
    vkb::Instance vkbInstance;
    vkb::PhysicalDevice vkbPhysicalDevice;
    vkb::Device vkbDevice;

    void CreateInstance();
    void CreateSurface();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void CreateQueues();
    void CreateAllocator();
};