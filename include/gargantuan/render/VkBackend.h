#pragma once

#include <SDL3/SDL.h>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>

constexpr unsigned int FRAME_OVERLAP = 2;
struct Frame
{
    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;
    VkSemaphore swapchainSemaphore, renderSemaphore;
    VkFence renderFence;
};

class VkBackend
{
public:
    VkBackend();
    VkBackend(const VkBackend &) = delete;
    VkBackend &operator=(const VkBackend &) = delete;

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

    VkSwapchainKHR swapchain;
    VkFormat swapchainImageFormat;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

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

    void CreateSwapchain();
    void DestroySwapchain();

    void DestroyCommandPool();
};