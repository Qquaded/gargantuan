#include "gargantuan/render/vulkan/VkSwapchain.h"
#include "gargantuan/render/vulkan/VkAssert.h"

#include <spdlog/spdlog.h>

VkSwapchain::VkSwapchain(vkb::Device& vkbDevice, VkSurfaceKHR surface)
{
    this->vkbDevice = vkbDevice;
    this->surface = surface;
    this->Create();
}

void VkSwapchain::Create()
{
    spdlog::trace("VkSwapchain: Creating");

    vkb::SwapchainBuilder swapchainBuilder(vkbDevice, surface);

    auto swapchainResult =
        swapchainBuilder.set_desired_format(VkSurfaceFormatKHR{.format = imageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            .build();

    vkbSwapchain = assertVkbResult(swapchainResult, "Failed to instantiate swapchain");

    swapchain = vkbSwapchain.swapchain;
    images = vkbSwapchain.get_images().value();
    imageViews = vkbSwapchain.get_image_views().value();
    extent = vkbSwapchain.extent;
}

void VkSwapchain::Destroy()
{
    spdlog::trace("VkSwapchain: Destroying");

    auto device = this->vkbDevice.device;
    for (auto& imageView : imageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkb::destroy_swapchain(vkbSwapchain);
}
