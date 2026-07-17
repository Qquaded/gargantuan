
#include "gargantuan/render/vulkan/VkBackend.h"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

int main()
{
    spdlog::trace("VkBackend: Initializing SDL");
    SDL_Init(SDL_INIT_VIDEO);

    spdlog::trace("VkBackend: Creating window");
    auto window = SDL_CreateWindow("Gargantuan", 480, 320, SDL_WINDOW_VULKAN); //| SDL_WINDOW_RESIZABLE);

    spdlog::set_level(spdlog::level::trace);

    VkBackend backend(window);

    SDL_Event event;
    bool isRunning = true;
    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;

            default:
                break;
            }
        }

        backend.DrawFrame();
    }

    backend.Destroy();

    return 0;
}
