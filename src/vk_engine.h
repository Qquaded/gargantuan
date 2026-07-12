#pragma once

#include "vk_types.h"

class VulkanEngine {
public:
	bool isInitialized { false };
	bool isRendering { true };
	int frameNumber { 0 };

	VkExtent2D windowExtent { 480, 320 };
	struct SDL_Window* window { nullptr };

	void init();
	void cleanup();
	void draw();
	void run();
};
