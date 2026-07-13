#pragma once

#include "../../VkBootstrap.h"

#include "SDL2/SDL.h"
#include "vulkan/vulkan.h"

class VulkanBackend
{
public:
    bool isPostInitialized{false};

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily = 0;

    VkSwapchainKHR swapchain;
    VkFormat swapchainImageFormat;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    void createInstance();
    void initPostInstance(VkSurfaceKHR s);
    void destroy();

private:
    vkb::Instance vkbInstance;
    vkb::PhysicalDevice vkbPhysicalDevice;
    vkb::Device vkbLogicalDevice;
    vkb::Swapchain vkbSwapchain;

    void createDevices();
    void createQueues();
    void createSwapchain();
    void destroySwapchain();
};