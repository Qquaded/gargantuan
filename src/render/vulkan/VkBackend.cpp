#define VMA_IMPLEMENTATION

#include "gargantuan/render/vulkan/VkBackend.h"
#include "gargantuan/render/vulkan/VkAssert.h"
#include "gargantuan/render/vulkan/VkImages.h"
#include "gargantuan/render/vulkan/VkResources.h"
#include "gargantuan/render/vulkan/VkStruct.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>
#include <VkBootstrap.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* pUserData
)
{
    spdlog::level::level_enum logLevel = spdlog::level::level_enum::trace;
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        logLevel = spdlog::level::level_enum::err;
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        logLevel = spdlog::level::level_enum::warn;
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        logLevel = spdlog::level::level_enum::info;
    };

    spdlog::log(logLevel, callbackData->pMessage);

    return VK_FALSE;
}

VkBackend::VkBackend(SDL_Window* window)
{
    this->window = window;

    this->CreateInstance();
    this->CreateSurface();
    this->SelectPhysicalDevice();
    this->CreateLogicalDevice();
    this->CreateAllocator();

    this->swapchain = std::make_unique<VkSwapchain>(this->vkbDevice, this->surface);
    this->resources = VkResources(device, allocator);

    this->CreateQueues();
    this->CreateCommandPool();
    this->CreateDrawImage();

    spdlog::trace("VkBackend: Finished constructing");
}

void VkBackend::DrawImage(VkCommandBuffer cmd)
{
    VkClearColorValue clearValue;
    float flash = std::abs(std::sin(frameCount / 120.0f));
    clearValue = {{0.0f, 0.0f, flash, 1.0f}};

    auto clearRange = VkStruct::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
}

void VkBackend::DrawFrame()
{
    auto& currentFrame = GetCurrentFrame();
    auto renderFence = currentFrame.renderFence;

    assertVkResult(vkWaitForFences(device, 1, &renderFence, true, 1000000000));
    assertVkResult(vkResetFences(device, 1, &renderFence));

    currentFrame.resources.Flush();

    spdlog::trace("1");

    uint32_t swapchainImageIndex;
    auto acquireNextImageResult =
        vkAcquireNextImageKHR(device, this->swapchain->swapchain, 1000000000, currentFrame.swapchainSemaphore, nullptr, &swapchainImageIndex);


    // if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR || acquireNextImageResult == VK_SUBOPTIMAL_KHR)
    // {
    //     // FIXME: should recreate all frames' render semaphores here, but im lazy
    //     vkDeviceWaitIdle(device);
    //     this->swapchain.Destroy();
    //     this->swapchain.Create();
    //     return;
    // }
    // else
    // {
    assertVkResult(acquireNextImageResult);
    // }

    // assertVkResult(vkResetFences(device, 1, &renderFence));

    spdlog::trace("2");

    VkCommandBuffer cmd = currentFrame.mainCommandBuffer;
    assertVkResult(vkResetCommandBuffer(cmd, 0));

    spdlog::trace("3");

    auto commandBeginInfo = VkStruct::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    assertVkResult(vkBeginCommandBuffer(cmd, &commandBeginInfo));

    spdlog::trace("4");

    auto currentImage = this->swapchain->images[swapchainImageIndex];

    drawExtent.width = drawImage.imageExtent.width;
    drawExtent.height = drawImage.imageExtent.height;


    VkImages::TransitionImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    DrawImage(cmd);

    VkImages::TransitionImage(cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    VkImages::TransitionImage(cmd, swapchain->images[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImages::CopyImageToImage(cmd, drawImage.image, swapchain->images[swapchainImageIndex], drawExtent, swapchain->extent);

    VkImages::TransitionImage(cmd, swapchain->images[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    spdlog::trace("5");

    assertVkResult(vkEndCommandBuffer(cmd));

    spdlog::trace("6");

    // Submission

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &currentFrame.swapchainSemaphore;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &currentFrame.renderSemaphore;

    assertVkResult(vkQueueSubmit(graphicsQueue, 1, &submitInfo, currentFrame.renderFence));

    spdlog::trace("7");

    // Presentation

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &(this->swapchain->swapchain);
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &currentFrame.renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    assertVkResult(vkQueuePresentKHR(graphicsQueue, &presentInfo));

    frameCount++;

    spdlog::trace("8");
}

void VkBackend::CreateInstance()
{
    spdlog::trace("VkBackend: Creating instance");
    vkb::InstanceBuilder instanceBuilder;

    uint32_t sdlExtensionCount;
    auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    for (uint32_t i = 0; i < sdlExtensionCount; i++)
    {
        instanceBuilder.enable_extension(sdlExtensions[i]);
    }

    auto instanceResult = instanceBuilder.set_app_name("Gargantuan")
                              .require_api_version(1, 0, 0)
#ifdef NDEBUG
#else
                              .request_validation_layers()
                              .set_debug_callback(vkDebugCallback)
#endif
#ifdef __APPLE__
                              .enable_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
#endif
                              .build();

    vkbInstance = assertVkbResult(instanceResult, "Failed to instantiate Vulkan instance");
    instance = vkbInstance.instance;
}

void VkBackend::CreateSurface()
{
    spdlog::trace("VkBackend: Creating surface");
    SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
}

void VkBackend::SelectPhysicalDevice()
{
    spdlog::trace("VkBackend: Selecting physical device");


    vkb::PhysicalDeviceSelector physicalSelector(vkbInstance);
    auto selectorResult = physicalSelector.set_surface(surface).set_minimum_version(1, 0).select();

    vkbPhysicalDevice = assertVkbResult(selectorResult, "Failed to instantiate physical device");
    physicalDevice = vkbPhysicalDevice.physical_device;

    spdlog::trace("VkBackend: Selected physical device {}", vkbPhysicalDevice.name);
}

void VkBackend::CreateLogicalDevice()
{
    spdlog::trace("VkBackend: Creating logical device");

    vkb::DeviceBuilder deviceBuilder(vkbPhysicalDevice);
    auto deviceResult = deviceBuilder.build();

    vkbDevice = assertVkbResult(deviceResult, "Failed to create logical device");
    device = vkbDevice.device;
}

void VkBackend::CreateAllocator()
{
    spdlog::trace("VkBackend: Creating allocator");

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_0;
    allocatorCreateInfo.physicalDevice = physicalDevice;
    allocatorCreateInfo.device = device;
    allocatorCreateInfo.instance = instance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    vmaCreateAllocator(&allocatorCreateInfo, &allocator);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = 65536;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocationInfo{};
    allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

    vmaCreateBuffer(allocator, &bufferInfo, &allocationInfo, &buffer, &allocation, nullptr);
}

void VkBackend::CreateQueues()
{
    spdlog::trace("VkBackend: Creating queues");

    graphicsQueue = assertVkbResult(vkbDevice.get_queue(vkb::QueueType::graphics), "Failed to get graphics queue");
    graphicsQueueFamily = assertVkbResult(vkbDevice.get_queue_index(vkb::QueueType::graphics), "Failed to get graphics queue index");
}

void VkBackend::CreateDrawImage()
{
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);

    VkExtent3D drawImageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};

    drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawImage.imageExtent = drawImageExtent;

    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto imageInfo = VkStruct::ImageCreateInfo(drawImage.imageFormat, drawImageUsages, drawImageExtent);

    VmaAllocationCreateInfo imageAllocationInfo{};
    imageAllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    imageAllocationInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    assertVkResult(vmaCreateImage(allocator, &imageInfo, &imageAllocationInfo, &drawImage.image, &drawImage.allocation, nullptr));
    resources.Push(drawImage.image, drawImage.allocation);

    auto imageViewInfo = VkStruct::ImageViewCreateInfo(drawImage.imageFormat, drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

    assertVkResult(vkCreateImageView(device, &imageViewInfo, nullptr, &drawImage.imageView));
    resources.Push(drawImage.imageView);
}

void VkBackend::CreateCommandPool()
{
    auto commandPoolInfo = VkStruct::CommandPoolCreateInfo(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    auto fenceCreateInfo = VkStruct::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    auto semaphoreCreateInfo = VkStruct::SemaphoreCreateInfo();

    for (auto& frame : frames)
    {
        assertVkResult(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frame.commandPool));

        auto commandAllocateInfo = VkStruct::CommandPoolAllocateInfo(frame.commandPool);
        assertVkResult(vkAllocateCommandBuffers(device, &commandAllocateInfo, &frame.mainCommandBuffer));

        assertVkResult(vkCreateFence(device, &fenceCreateInfo, nullptr, &frame.renderFence));
        assertVkResult(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.renderSemaphore));
        assertVkResult(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.swapchainSemaphore));
    }
}

void VkBackend::Destroy()
{
    spdlog::trace("VkBackend: Destroying");

    vkDeviceWaitIdle(device);

    for (auto& frame : frames)
    {
        vkDestroyCommandPool(device, frame.commandPool, nullptr);
        vkDestroyFence(device, frame.renderFence, nullptr);
        vkDestroySemaphore(device, frame.renderSemaphore, nullptr);
        vkDestroySemaphore(device, frame.swapchainSemaphore, nullptr);
        frame.resources.Flush();
    };
    resources.Flush();

    vmaDestroyBuffer(allocator, buffer, allocation);
    vmaDestroyAllocator(allocator);

    swapchain->Destroy();

    vkb::destroy_surface(instance, surface);
    vkb::destroy_device(vkbDevice);
    vkb::destroy_instance(vkbInstance);

    SDL_DestroyWindow(window);
}
