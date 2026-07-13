#include "types.h"

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  bool isSupported() { return graphicsFamily.has_value(); }
};

class Core {
public:
  bool isRendering{true};
  bool isRunning{true};
  int frameNumber{0};

  VkExtent2D windowExtent{480, 320};
  struct SDL_Window *window{nullptr};

  VkInstance instance;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  void initSDL();
  void initVulkan();
  void cleanup();
  void render();
  void handleEvent(SDL_Event event);
  void runLoop();

private:
  bool isValidationLayersSupported(
      const std::vector<const char *> &validationLayers);
  void initVulkanInstance();

  QueueFamilyIndices getQueueFamilies(VkPhysicalDevice physicalDevice);
  int scorePhysicalDevice(VkPhysicalDevice physicalDevice);
  void initPhysicalDevice();
};
