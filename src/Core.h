#include "types.h"
#include "vulkan/vulkan_core.h"
#include <vector>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  bool isSupported() { return graphicsFamily.has_value(); }
};

class Core {
public:
  const std::vector<const char *> REQUIRED_INSTANCE_EXTENSIONS{};
  const std::vector<const char *> REQUIRED_DEVICE_EXTENSIONS{};
  const std::vector<const char *> OPTIONAL_DEVICE_EXTENSIONS{
  	"VK_KHR_portability_subset"
  };

  bool isRendering{true};
  bool isRunning{true};
  int frameNumber{0};

  VkExtent2D windowExtent{480, 320};
  struct SDL_Window *window{nullptr};

  VkInstance instance;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;

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
  std::vector<VkExtensionProperties> getPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice);
  bool isPhysicalDeviceSupportsExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions);
  int scorePhysicalDevice(VkPhysicalDevice physicalDevice);
  void initPhysicalDevice();

  void initLogicalDevice();
};
