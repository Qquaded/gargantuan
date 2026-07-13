#pragma once

#include "../VkBootstrap.h"
#include "./vulkan/VulkanBackend.h"

#include "vulkan/vulkan.h"

constexpr unsigned int FRAME_OVERLAP = 2;
struct Frame
{
    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;
};

class Renderer
{
public:
    Renderer(VulkanBackend* backend);

    int frameCount = 0;
    Frame frames[FRAME_OVERLAP];
    Frame getCurrentFrame()
    {
        return frames[frameCount % FRAME_OVERLAP];
    };

    void init();
    void destroy();
    void render();

private:
    VulkanBackend* backend = nullptr;
};