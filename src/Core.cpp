#include "Core.h"
#include "SDL.h"
#include "vulkan/vulkan_core.h"

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

void Core::initSDL() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags windowFlags = SDL_WindowFlags(SDL_WINDOW_VULKAN);

  window = SDL_CreateWindow("Gargantuan", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, windowExtent.width,
                            windowExtent.height, windowFlags);

  if (window == nullptr) {
    throw std::runtime_error(std::string("Failed to instantiate window: ") +
                             SDL_GetError());
  }
}

void Core::initVulkan() {
  initVulkanInstance();
  initPhysicalDevice();
}

void Core::cleanup() {
  vkDestroyInstance(instance, nullptr);
  SDL_DestroyWindow(window);
}

void Core::render() {
  // TBA
}

void Core::runLoop() {
  SDL_Event event;

  while (isRunning) {
    while (SDL_PollEvent(&event)) {
      handleEvent(event);
    }

    if (isRunning && isRendering) {
      render();
      continue;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void Core::handleEvent(SDL_Event event) {
  switch (event.type) {
  case SDL_QUIT:
    isRunning = false;
    break;

  case SDL_WINDOWEVENT_MINIMIZED:
    isRendering = false;
    break;

  case SDL_WINDOWEVENT_MAXIMIZED:
    isRendering = true;
    break;

  case SDL_KEYUP:
  case SDL_KEYDOWN: {
    SDL_KeyboardEvent key = event.key;
    printf(key.type == SDL_KEYUP ? "Released" : "Pressed");
    printf(": %s", SDL_GetKeyName(event.key.keysym.sym));
    printf("\n");
    break;
  }

  default:
    break;
  }
}

void Core::initVulkanInstance() {
  VkApplicationInfo applicationInfo{};
  applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.pApplicationName = "Gargantuan";
  applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  applicationInfo.pEngineName = "No Engine";
  applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &applicationInfo;

  uint32_t extensionCount;
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
  std::vector<const char *> extensionNames(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(window, &extensionCount,
                                   extensionNames.data());

  // Required for Apple Silicon
  extensionNames.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  extensionCount += 1;

  createInfo.enabledExtensionCount = extensionCount;
  createInfo.ppEnabledExtensionNames = extensionNames.data();

#if NDEBUG
  createInfo.enabledLayerCount = 0;
#else
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  if (!isValidationLayersSupported(validationLayers)) {
    throw std::runtime_error("Validation layers unavailable");
  };

  createInfo.enabledLayerCount = validationLayers.size();
  createInfo.ppEnabledLayerNames = validationLayers.data();
#endif

  VkResult createResult = vkCreateInstance(&createInfo, nullptr, &instance);
  if (createResult != VK_SUCCESS) {
    std::string errorMessage = "Failed to create Vulkan instance: ";
    errorMessage.append(string_VkResult(createResult));
    throw std::runtime_error(errorMessage);
  }
}

bool Core::isValidationLayersSupported(
    const std::vector<const char *> &validationLayers) {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound)
      return false;
  }

  return true;
}

// Physical device

QueueFamilyIndices Core::getQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilyProperties.data());

  for (int i = 0; i < queueFamilyCount; i++) {
    if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    if (indices.isSupported())
      break;
  }

  return indices;
}

int Core::scorePhysicalDevice(VkPhysicalDevice device) {
  int score = 0;

  if (!getQueueFamilies(device).isSupported())
    return 0;

  VkPhysicalDeviceFeatures deviceFeatures;
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  // Metal doesn't use geometry shaders, so MoltenVK reports false here.
  // We should still score higher for devices that support it.
  if (deviceFeatures.geometryShader)
    score += 500;

  // Maximum possible size of textures affects graphics quality
  score += deviceProperties.limits.maxImageDimension2D;

  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    score += 1000;

  return score;
}

void Core::initPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0)
    throw std::runtime_error("No devices available");
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  std::multimap<int, VkPhysicalDevice> candidates;
  for (const auto &device : devices) {
    int score = scorePhysicalDevice(device);
    candidates.insert(std::make_pair(score, device));
  }

  if (candidates.rbegin()->first > 0)
    physicalDevice = candidates.rbegin()->second;
  else
    throw std::runtime_error("No supported devices");
}
