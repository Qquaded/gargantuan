#pragma once

#include <SDL3/SDL.h>

#include <VkBootstrap.h>

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

class VkBackend
{
public:
    VkBackend();

    SDL_Window* window;

    VkInstance instance;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily = 0;

    void Destroy();

private:
    vkb::Instance vkbInstance;
    vkb::PhysicalDevice vkbPhysicalDevice;
    vkb::Device vkbDevice;

    void CreateInstance();
    void CreateSurface();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void CreateQueues();
};