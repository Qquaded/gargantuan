#include "../VkBootstrap.h"
#include "SDL2/SDL.h"
#include "vulkan/vulkan.h"

class Renderer {
public:
  bool isInitialized{false};
  bool isRunning{true};
  bool isRendering{false};

  vkb::Instance vkbInstance;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  SDL_Window *window;
  VkSurfaceKHR surface;

  vkb::PhysicalDevice vkbPhysicalDevice;
  VkPhysicalDevice physicalDevice;

  vkb::Device vkbLogicalDevice;
  VkDevice logicalDevice;

  VkQueue graphicsQueue;

  void init();
  void destroy();
  void render();
  void handleEvent(SDL_Event event);

private:
  void createInstance();
  void createSurface();
  void createDevices();
  void createQueues();
};