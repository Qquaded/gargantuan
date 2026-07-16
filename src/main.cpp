
#include "gargantuan/render/VkBackend.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

int main()
{
    spdlog::set_level(spdlog::level::trace);

    VkBackend backend;

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
    }

    backend.Destroy();

    return 0;
}
