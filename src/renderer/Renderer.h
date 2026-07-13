#include "../VkBootstrap.h"
#include "SDL2/SDL.h"
#include "vulkan/vulkan.h"

constexpr unsigned int FRAME_OVERLAP = 2;
struct Frame {
  VkCommandPool commandPool;
  VkCommandBuffer mainCommandBuffer;
};

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

  VkSwapchainKHR swapchain;
  vkb::Swapchain vkbSwapchain;
  VkFormat swapchainImageFormat;
  std::vector<VkImage> swapchainImages;
  std::vector<VkImageView> swapchainImageViews;

  VkCommandPool commandPool;
  VkCommandBuffer mainCommandBuffer;

  int frameCount = 0;
  Frame frames[FRAME_OVERLAP];
  Frame getCurrentFrame() { return frames[frameCount % FRAME_OVERLAP]; };

  VkQueue graphicsQueue;
  uint32_t graphicsQueueFamily;

  void init();
  void destroy();
  void render();
  void handleEvent(SDL_Event event);

private:
  void createInstance();
  void createSurface();
  void createDevices();
  void createQueues();
  void createSwapchain();
  void destroySwapchain();
  void initCommands();
};