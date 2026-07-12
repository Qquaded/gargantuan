#include "vk_engine.h"

#include "SDL2/SDL.h"

void VulkanEngine::init()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags windowFlags = SDL_WindowFlags(SDL_WINDOW_VULKAN);

	window = SDL_CreateWindow(
		"Gargantuan",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		windowExtent.width,
		windowExtent.height,
		windowFlags
	);

	isInitialized = true;
}

void VulkanEngine::cleanup()
{
	if (isInitialized)
	{
		SDL_DestroyWindow(window);
	}
}

void VulkanEngine::draw()
{
	// TBA
}

void VulkanEngine::run()
{
	SDL_Event event;
	bool quit = false;

	while (true) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					return;

				case SDL_WINDOWEVENT_MINIMIZED:
					isRendering = false;
					break;

				case SDL_WINDOWEVENT_MAXIMIZED:
					isRendering = true;
					break;

				case SDL_KEYUP:
				case SDL_KEYDOWN: {
					SDL_KeyboardEvent key = event.key;
					printf(key.type == SDL_KEYUP ? "Pressed" : "Released");
					printf(": %s", SDL_GetKeyName(event.key.keysym.sym));
					printf("\n");
				}

				default:
					break;
			}
		}

		if (isRendering) {
			draw();
			continue;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
