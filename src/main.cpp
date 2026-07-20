#include "gargantuan/Engine.hpp"
#include "gargantuan/instances/ClassRegistry.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(SDL_Quit);

    auto &map = gargantuan::instances::ClassRegistry::GetDefinitionsMap();
    for (auto def : map) {
        SDL_Log("Got class definition %s -> %s", def.first.name(), def.second.Name.data());
    };

    gargantuan::Engine engine;
    while (engine.IsRunning) {
        engine.Step();
    }

    return 0;
}
