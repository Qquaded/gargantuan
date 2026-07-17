#pragma once

#include <vector>
#include <VkBootstrap.h>
#include <vulkan/vulkan.h>

class VkSwapchain
{
public:
    VkSwapchain() = default;
    VkSwapchain(vkb::Device& vkbDevice, VkSurfaceKHR surface);

    void Create();
    void Destroy();

    vkb::Device vkbDevice;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    vkb::Swapchain vkbSwapchain;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat imageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    VkExtent2D extent;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};
