#include "Renderer.h"
#include "../VkBootstrap.h"
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vulkan.h"

template <typename T>
T unwrapVkbResult(vkb::Result<T> result, const char *baseMessage) {
  if (result) {
    return result.value();
  }

  std::string errorMessage(baseMessage);
  errorMessage.append(": ");
  errorMessage.append(result.error().message());

  auto failureReasons = result.detailed_failure_reasons();
  if (failureReasons.size() != 0) {
    for (const auto &reason : failureReasons) {
      errorMessage.append("\n\t");
      errorMessage.append(reason.c_str());
    }
  }

  throw std::runtime_error(errorMessage);
}

void Renderer::init() {
  createInstance();
  createSurface();
  createDevices();
  createQueues();

  isInitialized = true;
}

void Renderer::destroy() {
  if (isInitialized) {
    vkb::destroy_surface(vkbInstance, surface);
    vkb::destroy_device(vkbLogicalDevice);
    vkb::destroy_instance(vkbInstance);
    SDL_DestroyWindow(window);
  }
}

void Renderer::render() {}

void Renderer::handleEvent(SDL_Event event) {
  switch (event.type) {
  case SDL_QUIT:
    isRunning = false;
    break;

  case SDL_KEYUP:
  case SDL_KEYDOWN: {
    printf("\n%s %s", event.type == SDL_KEYUP ? "Released" : "Pressed",
           SDL_GetKeyName(event.key.keysym.sym));
    break;
  }

  case SDL_WINDOWEVENT_MINIMIZED:
    isRendering = false;
    break;

  case SDL_WINDOWEVENT_MAXIMIZED:
    isRendering = true;
    break;

  default:
    break;
  }
}

void Renderer::createInstance() {
  vkb::InstanceBuilder instanceBuilder;
  auto instanceResult =
      instanceBuilder.set_app_name("Gargantuan")
          .request_validation_layers()
          .enable_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
          .add_flag(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR)
          .use_default_debug_messenger()
          .build();

  vkbInstance =
      unwrapVkbResult(instanceResult, "Failed to create Vulkan instance");

  instance = vkbInstance.instance;
  debugMessenger = vkbInstance.debug_messenger;
}

void Renderer::createSurface() {
  SDL_Init(SDL_INIT_VIDEO);
  window =
      SDL_CreateWindow("Gargantuan", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 480, 320, SDL_WINDOW_VULKAN);
  SDL_Vulkan_CreateSurface(window, instance, &surface);
}

void Renderer::createDevices() {
  vkb::PhysicalDeviceSelector physicalSelector(vkbInstance);
  auto selectorResult =
      physicalSelector.set_surface(surface).set_minimum_version(1, 0).select();
  vkbPhysicalDevice =
      unwrapVkbResult(selectorResult, "Failed to instantiate physical device");
  physicalDevice = vkbPhysicalDevice.physical_device;

  vkb::DeviceBuilder logicalBuilder(vkbPhysicalDevice);
  auto logicalResult = logicalBuilder.build();
  vkbLogicalDevice =
      unwrapVkbResult(logicalResult, "Failed to create logical device");
  logicalDevice = vkbLogicalDevice.device;
}

void Renderer::createQueues() {
  auto graphicsResult = vkbLogicalDevice.get_queue(vkb::QueueType::graphics);
  graphicsQueue =
      unwrapVkbResult(graphicsResult, "Failed to create graphics queue");
}