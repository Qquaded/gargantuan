#include "Renderer.h"
#include "../VkBootstrap.h"
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vk_enum_string_helper.h"
#include "vulkan/vulkan.h"

template <typename T>
T assertVkbResult(vkb::Result<T> result, const char *baseMessage) {
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

void assertVkResult(VkResult result) {
  if (result != VK_SUCCESS) {
    throw std::runtime_error(string_VkResult(result));
  }
}

void Renderer::init() {
  createInstance();
  createSurface();
  createDevices();
  createQueues();
  createSwapchain();
  initCommands();

  isInitialized = true;
}

void Renderer::destroy() {
  if (isInitialized) {
    vkDeviceWaitIdle(logicalDevice);
    for (int i = 0; i < FRAME_OVERLAP; i++) {
      vkDestroyCommandPool(logicalDevice, frames[i].commandPool, nullptr);
    }

    destroySwapchain();

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
      assertVkbResult(instanceResult, "Failed to create Vulkan instance");

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
  VkPhysicalDeviceVulkan13Features features13{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
  features13.dynamicRendering = true;
  features13.synchronization2 = true;

  VkPhysicalDeviceVulkan12Features features12{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  features12.bufferDeviceAddress = true;
  features12.descriptorIndexing = true;

  vkb::PhysicalDeviceSelector physicalSelector(vkbInstance);
  auto selectorResult = physicalSelector.set_surface(surface)
                            .set_minimum_version(1, 0)
                            .set_required_features_13(features13)
                            .set_required_features_12(features12)
                            .select();
  vkbPhysicalDevice =
      assertVkbResult(selectorResult, "Failed to instantiate physical device");
  physicalDevice = vkbPhysicalDevice.physical_device;

  vkb::DeviceBuilder logicalBuilder(vkbPhysicalDevice);
  auto logicalResult = logicalBuilder.build();
  vkbLogicalDevice =
      assertVkbResult(logicalResult, "Failed to create logical device");
  logicalDevice = vkbLogicalDevice.device;
}

void Renderer::createQueues() {
  graphicsQueue = vkbLogicalDevice.get_queue(vkb::QueueType::graphics).value();
  graphicsQueueFamily =
      vkbLogicalDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void Renderer::createSwapchain() {
  vkb::SwapchainBuilder swapchainBuilder(physicalDevice, logicalDevice,
                                         surface);
  swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

  vkbSwapchain = swapchainBuilder
                     .set_desired_format(VkSurfaceFormatKHR{
                         .format = swapchainImageFormat,
                         .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                     .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                     .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                     .build()
                     .value();

  swapchain = vkbSwapchain.swapchain;
  swapchainImages = vkbSwapchain.get_images().value();
  swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void Renderer::destroySwapchain() {
  vkb::destroy_swapchain(vkbSwapchain);
  for (int i = 0; i < swapchainImageViews.size(); i++) {
    vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
  }
}

void Renderer::initCommands() {
  VkCommandPoolCreateInfo commandPoolInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = graphicsQueueFamily};

  for (int i = 0; i < FRAME_OVERLAP; i++) {
    assertVkResult(vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr,
                                       &frames[i].commandPool));

    VkCommandBufferAllocateInfo commandAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = frames[i].commandPool,
        .commandBufferCount = 1,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY};

    assertVkResult(vkAllocateCommandBuffers(logicalDevice, &commandAllocateInfo,
                                            &frames[i].mainCommandBuffer));
  }
}