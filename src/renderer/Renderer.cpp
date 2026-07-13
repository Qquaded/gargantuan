#include "Renderer.h"

#include "../VkBootstrap.h"
#include "./vulkan/VulkanBackend.h"
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

    for (int i = 0; i < FRAME_OVERLAP; i++)
    {
        VulkanUtils::assertVkResult(vkCreateCommandPool(backend->logicalDevice, &commandPoolInfo, nullptr, &frames[i].commandPool));

        auto commandAllocateInfo = VulkanUtils::createCommandPoolAllocateInfo(frames[i].commandPool);
        VulkanUtils::assertVkResult(vkAllocateCommandBuffers(backend->logicalDevice, &commandAllocateInfo, &frames[i].mainCommandBuffer));
    }
}

void Renderer::render() {}

void Renderer::destroy()
{
    auto logicalDevice = backend->logicalDevice;
    vkDeviceWaitIdle(logicalDevice);
    for (int i = 0; i < FRAME_OVERLAP; i++)
    {
        vkDestroyCommandPool(logicalDevice, frames[i].commandPool, nullptr);
    }
}
