#include "vk_engine.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "vulkan/vk_enum_string_helper.h"


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

	VkApplicationInfo applicationInfo {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Gargantuan";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	// Get SDL extensions
	uint32_t sdlExtensionCount;
	SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr);
	const char** sdlExtensionNames = new const char *[sdlExtensionCount];
	SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, sdlExtensionNames);

	// Add in KHR extension
	std::vector<const char*> requiredExtensions;
	for (uint32_t index = 0; index < sdlExtensionCount; index++) {
		requiredExtensions.emplace_back(requiredExtensions[index]);
	}

	requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	createInfo.enabledExtensionCount = requiredExtensions.size();
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	createInfo.enabledLayerCount = 0;

	VkResult createResult = vkCreateInstance(&createInfo, nullptr, &instance);
	if (createResult != VK_SUCCESS) {
		std::string errorMessage = "Failed to create Vulkan instance: ";
		errorMessage.append(string_VkResult(createResult));
		throw std::runtime_error(errorMessage);
	}

	isInitialized = true;
}

void VulkanEngine::cleanup()
{
	if (isInitialized)
	{
		vkDestroyInstance(instance, nullptr);
		SDL_DestroyWindow(window);
	}
}

void VulkanEngine::render()
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
			render();
			continue;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
