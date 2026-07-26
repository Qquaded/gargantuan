#include "gargantuan/Engine.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <cstdlib>
#include <spdlog/spdlog.h>

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	std::atexit(SDL_Quit);

	gargantuan::Engine engine;
	while (engine.IsRunning) {
		engine.Step();
	}

	return 0;
}
