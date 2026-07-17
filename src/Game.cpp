#include "gargantuan/Game.hpp"
#include "gargantuan/render/Renderer.hpp"
#include <SDL3/SDL_events.h>

namespace gargantuan {

Game::Game()
    : Window(SDL_CreateWindow("Gargantuan", 720, 540, SDL_WINDOW_RESIZABLE)),
      Renderer(Window) {}

Game::~Game() { SDL_DestroyWindow(Window); }

void Game::ProcessEvent(SDL_Event event) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
        SDL_Log("Stopping");
        IsRunning = false;
        return;

    case SDL_EVENT_KEY_UP:
    case SDL_EVENT_KEY_DOWN: {
        auto verb = event.type == SDL_EVENT_KEY_UP ? "Released" : "Pressed";
        SDL_Log("%s %s", verb, SDL_GetKeyName(event.key.key));
    } break;

    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
    } break;
    }
}

void Game::Step() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ProcessEvent(event);
    }

    Renderer.Draw();
}

} // namespace gargantuan
