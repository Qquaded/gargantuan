#include "gargantuan/Renderer.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <spdlog/spdlog.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    auto window =
        SDL_CreateWindow("Gargantuan", 720, 540, SDL_WINDOW_RESIZABLE);

    Gargantuan::Renderer renderer(window);

    SDL_Event event;
    bool isRunning = true;
    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                SDL_Log("Stopping");
                isRunning = false;
                break;

            // case SDL_EVENT_KEY_UP:
            case SDL_EVENT_KEY_DOWN:
                auto verb =
                    event.type == SDL_EVENT_KEY_UP ? "Released" : "Pressed";
                SDL_Log("%s %s", verb, SDL_GetKeyName(event.key.key));
                break;
            }
        }

        renderer.Draw();
    }

    return 0;
}
