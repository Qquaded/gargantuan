#include "gargantuan/render/VkBackend.h"

#include "gargantuan/render/VkAssert.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#include <VkBootstrap.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* pUserData
)
{
    spdlog::level::level_enum logLevel = spdlog::level::level_enum::trace;
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        logLevel = spdlog::level::level_enum::err;
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        logLevel = spdlog::level::level_enum::warn;
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        logLevel = spdlog::level::level_enum::info;
    };

    spdlog::log(logLevel, callbackData->pMessage);

    return VK_FALSE;
}

VkBackend::VkBackend()
{
    this->CreateInstance();
    this->CreateSurface();
    this->SelectPhysicalDevice();
    this->CreateLogicalDevice();
    this->CreateQueues();
    spdlog::trace("VkBackend: Finished constructing");
}

void VkBackend::CreateInstance()
{
    spdlog::trace("VkBackend: Initializing SDL");
    SDL_Init(SDL_INIT_VIDEO);

    spdlog::trace("VkBackend: Creating window");
    window = SDL_CreateWindow("Gargantuan", 480, 320, SDL_WINDOW_VULKAN);

    spdlog::trace("VkBackend: Creating instance");
    vkb::InstanceBuilder instanceBuilder;

    uint32_t sdlExtensionCount;
    auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    for (uint32_t i = 0; i < sdlExtensionCount; i++)
    {
        instanceBuilder.enable_extension(sdlExtensions[i]);
    }

    auto instanceResult = instanceBuilder.set_app_name("Gargantuan")
                              .require_api_version(1, 0, 0)
#ifdef NDEBUG
#else
                              .request_validation_layers()
                              .set_debug_callback(debugCallback)
#endif
#ifdef __APPLE__
                              .enable_extension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)
#endif
                              .build();

    vkbInstance = assertVkbResult(instanceResult, "Failed to instantiate Vulkan instance");
    instance = vkbInstance.instance;
}

void VkBackend::CreateSurface()
{
    spdlog::trace("VkBackend: Creating surface");
    SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
}

void VkBackend::SelectPhysicalDevice()
{
    spdlog::trace("VkBackend: Selecting physical device");

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector physicalSelector(vkbInstance);
    auto selectorResult = physicalSelector.set_surface(surface)
                              .set_minimum_version(1, 0)
                              .set_required_features_13(features13)
                              .set_required_features_12(features12)
                              .select();

    vkbPhysicalDevice = assertVkbResult(selectorResult, "Failed to instantiate physical device");
    physicalDevice = vkbPhysicalDevice.physical_device;

    spdlog::trace("VkBackend: Selected physical device {}", vkbPhysicalDevice.name);
}

void VkBackend::CreateLogicalDevice()
{
    spdlog::trace("VkBackend: Creating logical device");

    vkb::DeviceBuilder deviceBuilder(vkbPhysicalDevice);
    auto deviceResult = deviceBuilder.build();

    vkbDevice = assertVkbResult(deviceResult, "Failed to create logical device");
    device = vkbDevice.device;
}

void VkBackend::CreateQueues()
{
    spdlog::trace("VkBackend: Creating queues");

    graphicsQueue = assertVkbResult(vkbDevice.get_queue(vkb::QueueType::graphics), "Failed to get graphics queue");
    graphicsQueueFamily = assertVkbResult(vkbDevice.get_queue_index(vkb::QueueType::graphics), "Failed to get graphics queue index");
}

void VkBackend::Destroy()
{
    spdlog::trace("VkBackend: Destroying");
    vkb::destroy_surface(instance, surface);
    vkb::destroy_device(vkbDevice);
    vkb::destroy_instance(vkbInstance);
}