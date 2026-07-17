#include "gargantuan/Game.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(SDL_Quit);

    gargantuan::Game game;

    while (game.IsRunning) {
        game.Step();
    }

    return 0;
}
