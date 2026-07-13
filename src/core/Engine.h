#pragma once

#include "../renderer/Renderer.h"
#include "../renderer/vulkan/VulkanBackend.h"

#include "SDL2/SDL.h"

class Engine
{
public:
    bool isRunning{true};
    bool isRendering{true};

    SDL_Window* window{nullptr};
    std::unique_ptr<VulkanBackend> backend;
    std::unique_ptr<Renderer> renderer;

    void init();
    void step();
    void handleEvent(SDL_Event event);
    void destroy();
};