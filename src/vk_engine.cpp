#include "vk_engine.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

bool isValidationLayersSupported(std::vector<const char *> validationLayers) {
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

int scorePhysicalDevice(VkPhysicalDevice device) {
  int score = 0;

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

void VulkanEngine::init() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags windowFlags = SDL_WindowFlags(SDL_WINDOW_VULKAN);

  window = SDL_CreateWindow("Gargantuan", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, windowExtent.width,
                            windowExtent.height, windowFlags);

  if (window == nullptr) {
    throw std::runtime_error(std::string("Failed to create window: ") +
                             SDL_GetError());
  }

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

  // SDL extensions
  uint32_t sdlExtensionCount;
  SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr);
  const char **sdlExtensionNames = new const char *[sdlExtensionCount];
  SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount,
                                   sdlExtensionNames);

  // MacOS bullshittery
  std::vector<const char *> requiredExtensions;
  for (uint32_t index = 0; index < sdlExtensionCount; index++) {
    requiredExtensions.emplace_back(sdlExtensionNames[index]);
  }
  delete[] sdlExtensionNames;

  requiredExtensions.emplace_back(
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  // Validation layers
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  if (enableValidationLayers) {
    if (!isValidationLayersSupported(validationLayers)) {
      throw std::runtime_error("Validation layers unavailable");
    };

    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VkResult createResult = vkCreateInstance(&createInfo, nullptr, &instance);
  if (createResult != VK_SUCCESS) {
    std::string errorMessage = "Failed to create Vulkan instance: ";
    errorMessage.append(string_VkResult(createResult));
    throw std::runtime_error(errorMessage);
  }

  // Devices
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
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

  // TODO: queue families

  isInitialized = true;
}

void VulkanEngine::cleanup() {
  if (isInitialized) {
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
  }
}

void VulkanEngine::render() {
  // TBA
}

void VulkanEngine::run() {
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
        break;
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
