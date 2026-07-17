#pragma once

#include "gargantuan/render/vulkan/VkSwapchain.h"

#include <SDL3/SDL.h>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>

class VkBackend
{
public:
#if __APPLE__
    static const unsigned int FRAME_OVERLAP = 3;
#else
    static const unsigned int FRAME_OVERLAP = 2;
#endif

    struct Frame
    {
        VkCommandPool commandPool;
        VkCommandBuffer mainCommandBuffer;
        VkSemaphore swapchainSemaphore, renderSemaphore;
        VkFence renderFence;
    };

    VkBackend(SDL_Window* window);
    VkBackend(const VkBackend&) = delete;
    VkBackend& operator=(const VkBackend&) = delete;

    VkInstance instance;

    SDL_Window* window;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily = 0;

    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocator allocator;

    VkSwapchain swapchain;

    int frameCount = 0;
    Frame frames[FRAME_OVERLAP];
    Frame& GetCurrentFrame()
    {
        return frames[frameCount % FRAME_OVERLAP];
    };

    void DrawFrame();
    void Destroy();

private:
    vkb::Instance vkbInstance;
    vkb::PhysicalDevice vkbPhysicalDevice;
    vkb::Device vkbDevice;
    vkb::Swapchain vkbSwapchain;

    bool resizeRequested;

    void CreateInstance();
    void CreateSurface();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void CreateAllocator();
    void CreateQueues();

    void CreateCommandPool();
    void DestroyCommandPool();
};
