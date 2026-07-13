#include "Engine.h"

#include "SDL2/SDL_vulkan.h"

#include <chrono>
#include <thread>

void Engine::init()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Gargantuan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 320, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

    backend = std::make_unique<VulkanBackend>();
    backend->createInstance();

    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(window, backend->instance, &surface);
    backend->initPostInstance(surface);

    renderer = std::make_unique<Renderer>(backend.get());
    renderer->init();
}

void Engine::step()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        handleEvent(event);
    }

    if (isRendering)
    {
        renderer->render();
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Engine::handleEvent(SDL_Event event)
{
    switch (event.type)
    {
    case SDL_QUIT:
        isRunning = false;
        break;

    case SDL_KEYUP:
    case SDL_KEYDOWN:
    {
        printf("\n%s %s", event.type == SDL_KEYUP ? "Released" : "Pressed", SDL_GetKeyName(event.key.keysym.sym));
        break;
    }

    case SDL_WINDOWEVENT_MINIMIZED:
        isRendering = false;
        break;

    case SDL_WINDOWEVENT_MAXIMIZED:
        isRendering = true;
        break;

    default:
        break;
    }
}

void Engine::destroy()
{
    renderer->destroy();
    backend->destroy();
    SDL_DestroyWindow(window);
}