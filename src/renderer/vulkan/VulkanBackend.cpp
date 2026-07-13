#include "VulkanBackend.h"

#include "VulkanUtils.h"

void VulkanBackend::createInstance()
{
    assert(instance == VK_NULL_HANDLE);
    assert(!isPostInitialized);

    vkb::InstanceBuilder instanceBuilder;
    auto instanceResult = instanceBuilder.set_app_name("Gargantuan")
                              .request_validation_layers()
                              .enable_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
                              .add_flag(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR)
                              .use_default_debug_messenger()
                              .build();

    vkbInstance = VulkanUtils::assertVkbResult(instanceResult, "Failed to create Vulkan instance");

    instance = vkbInstance.instance;
    debugMessenger = vkbInstance.debug_messenger;
}

void VulkanBackend::initPostInstance(VkSurfaceKHR s)
{
    assert(!isPostInitialized);
    assert(instance != VK_NULL_HANDLE);
    surface = s;
    createDevices();
    createQueues();
    createSwapchain();
}

void VulkanBackend::destroy()
{
    destroySwapchain();

    vkb::destroy_surface(vkbInstance, surface);

    vkb::destroy_device(vkbLogicalDevice);
    vkb::destroy_instance(vkbInstance);
}

void VulkanBackend::createDevices()
{
    VkPhysicalDeviceVulkan13Features features13{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features12{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector physicalSelector(vkbInstance);
    auto selectorResult = physicalSelector.set_surface(surface)
                              .set_minimum_version(1, 0)
                              .set_required_features_13(features13)
                              .set_required_features_12(features12)
                              .select();
    vkbPhysicalDevice = VulkanUtils::assertVkbResult(selectorResult, "Failed to instantiate physical device");
    physicalDevice = vkbPhysicalDevice.physical_device;

    vkb::DeviceBuilder logicalBuilder(vkbPhysicalDevice);
    auto logicalResult = logicalBuilder.build();
    vkbLogicalDevice = VulkanUtils::assertVkbResult(logicalResult, "Failed to create logical device");
    logicalDevice = vkbLogicalDevice.device;
}

void VulkanBackend::createQueues()
{
    graphicsQueue = vkbLogicalDevice.get_queue(vkb::QueueType::graphics).value();
    graphicsQueueFamily = vkbLogicalDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanBackend::createSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder(physicalDevice, logicalDevice, surface);
    swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    vkbSwapchain =
        swapchainBuilder.set_desired_format(VkSurfaceFormatKHR{.format = swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

    swapchain = vkbSwapchain.swapchain;
    swapchainImages = vkbSwapchain.get_images().value();
    swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanBackend::destroySwapchain()
{
    vkb::destroy_swapchain(vkbSwapchain);
    for (int i = 0; i < swapchainImageViews.size(); i++)
    {
        vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
    }
}
