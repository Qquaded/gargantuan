#include "Renderer.h"

#include "../VkBootstrap.h"
#include "./vulkan/VulkanBackend.h"
#include "./vulkan/VulkanImages.h"
#include "./vulkan/VulkanUtils.h"

#include "SDL2/SDL_vulkan.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.h"

Renderer::Renderer(VulkanBackend* backend)
    : backend(backend)
{
}

void Renderer::init()
{
    auto commandPoolInfo = VulkanUtils::createCommandPoolCreateInfo(backend->graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    auto fenceCreateInfo = VulkanUtils::createFenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    auto semaphoreCreateInfo = VulkanUtils::createSemaphoreCreateInfo();


    for (int i = 0; i < FRAME_OVERLAP; i++)
    {
        auto logicalDevice = backend->logicalDevice;

        VulkanUtils::assertVkResult(vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &frames[i].commandPool));

        auto commandAllocateInfo = VulkanUtils::createCommandPoolAllocateInfo(frames[i].commandPool);
        VulkanUtils::assertVkResult(vkAllocateCommandBuffers(logicalDevice, &commandAllocateInfo, &frames[i].mainCommandBuffer));

        VulkanUtils::assertVkResult(vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &frames[i].renderFence));
        VulkanUtils::assertVkResult(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &frames[i].renderSemaphore));
        VulkanUtils::assertVkResult(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &frames[i].swapchainSemaphore));
    }
}

void Renderer::render()
{
    // auto logicalDevice = backend->logicalDevice;
    auto& currentFrame = getCurrentFrame();
    auto renderFence = currentFrame.renderFence;

    VulkanUtils::assertVkResult(vkWaitForFences(backend->logicalDevice, 1, &renderFence, true, 1000000000));
    VulkanUtils::assertVkResult(vkResetFences(backend->logicalDevice, 1, &renderFence));

    uint32_t swapchainImageIndex;
    VulkanUtils::assertVkResult(
        vkAcquireNextImageKHR(backend->logicalDevice, backend->swapchain, 1000000000, currentFrame.swapchainSemaphore, nullptr, &swapchainImageIndex)
    );

    VkCommandBuffer commands = currentFrame.mainCommandBuffer;
    VulkanUtils::assertVkResult(vkResetCommandBuffer(commands, 0));

    auto commandBeginInfo = VulkanUtils::createCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VulkanUtils::assertVkResult(vkBeginCommandBuffer(commands, &commandBeginInfo));

    printf("COMMAND BUFFER START\n");

    auto swapchainImages = backend->swapchainImages;
    auto currentImage = swapchainImages[swapchainImageIndex];

    printf("a\n");

    VulkanImages::transitionImage(commands, currentImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    printf("b\n");
    VkClearColorValue clearValue;
    float flash = std::abs(std::sin(frameCount / 120.0f));
    clearValue = {{0.0f, 0.0f, flash, 1.0f}};

    auto clearRange = VulkanUtils::createImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    vkCmdClearColorImage(commands, currentImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
    VulkanImages::transitionImage(commands, currentImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    printf("COMMAND BUFFER END\n");

    VulkanUtils::assertVkResult(vkEndCommandBuffer(commands));

    printf("COMMAND SUB START\n");

    // Submission

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &currentFrame.swapchainSemaphore;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commands;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &currentFrame.renderSemaphore;

    VulkanUtils::assertVkResult(vkQueueSubmit(backend->graphicsQueue, 1, &submitInfo, currentFrame.renderFence));
    printf("COMMAND SUB END\n");

    // Presentation

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &(backend->swapchain);
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &currentFrame.renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    VulkanUtils::assertVkResult(vkQueuePresentKHR(backend->graphicsQueue, &presentInfo));

    frameCount++;
}

void Renderer::destroy()
{
    auto logicalDevice = backend->logicalDevice;
    vkDeviceWaitIdle(logicalDevice);
    for (int i = 0; i < FRAME_OVERLAP; i++)
    {
        vkDestroyCommandPool(logicalDevice, frames[i].commandPool, nullptr);
        vkDestroyFence(logicalDevice, frames[i].renderFence, nullptr);
        vkDestroySemaphore(logicalDevice, frames[i].renderSemaphore, nullptr);
        vkDestroySemaphore(logicalDevice, frames[i].swapchainSemaphore, nullptr);
    }
}
