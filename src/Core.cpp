#include "Core.h"
#include "SDL.h"
#include "vulkan/vulkan_core.h"
#include <SDL_surface.h>
#include <cstdint>
#include <cstring>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

const std::vector<const char *> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"};

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
  initSurface();
  initPhysicalDevice();
  initLogicalDevice();
}

void Core::cleanup() {
  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
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
  applicationInfo.apiVersion = VK_API_VERSION_1_4;

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
  if (!isValidationLayersSupported(VALIDATION_LAYERS)) {
    throw std::runtime_error("Validation layers unavailable");
  };

  createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
  createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
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

  uint32_t presentFamily;
  VkBool32 presentSupport = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(device, presentFamily, surface,
                                       &presentSupport);
  if (presentSupport) {
    indices.presentFamily = presentFamily;
  }

  return indices;
}

std::vector<VkExtensionProperties>
Core::getPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> extensionProperties(extensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                       extensionProperties.data());
  return extensionProperties;
}

bool Core::isPhysicalDeviceSupportsExtensions(
    VkPhysicalDevice physicalDevice,
    const std::vector<const char *> &requiredExtensions) {
  auto extensionProperties = getPhysicalDeviceExtensions(physicalDevice);

  for (const char *required : requiredExtensions) {
    bool found = false;
    for (const auto &extension : extensionProperties) {
      if (strcmp(required, extension.extensionName) == 0) {
        found = true;
        break;
      }
    }

    if (!found)
      return false;
  }

  return true;
}

int Core::scorePhysicalDevice(VkPhysicalDevice device) {
  int score = 0;

  if (!getQueueFamilies(device).isSupported())
    return 0;

  if (!isPhysicalDeviceSupportsExtensions(device,
                                          Core::REQUIRED_DEVICE_EXTENSIONS))
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

// Logical devices

void Core::initLogicalDevice() {
  QueueFamilyIndices indices = getQueueFamilies(physicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = queueCreateInfos.size();
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  VkPhysicalDeviceFeatures deviceFeatures{};
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;

  std::vector<const char *> extensionNames = Core::REQUIRED_DEVICE_EXTENSIONS;
  auto availableExtensions = getPhysicalDeviceExtensions(physicalDevice);

  bool hasPortability = false;
  for (const auto &name : extensionNames) {
    if (strcmp(name, "VK_KHR_portability_subset") == 0) {
      hasPortability = true;
      break;
    }
  }
  if (!hasPortability)
    extensionNames.emplace_back("VK_KHR_portability_subset");

  createInfo.enabledExtensionCount = extensionNames.size();
  createInfo.ppEnabledExtensionNames = extensionNames.data();

  VkResult createResult =
      vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);

  std::string errorMessage = "Failed to create logical device: ";
  errorMessage.append(string_VkResult(createResult));
  if (createResult != VK_SUCCESS)
    throw std::runtime_error(errorMessage);

  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void Core::initSurface() {
  auto createResult = SDL_Vulkan_CreateSurface(window, instance, &surface);
  if (createResult != SDL_TRUE)
    throw std::runtime_error("Failed to instantiate surface");
}
