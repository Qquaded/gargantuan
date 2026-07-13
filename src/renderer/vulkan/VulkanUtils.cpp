#include "VulkanUtils.h"

#include "vulkan/vk_enum_string_helper.h"

namespace VulkanUtils
{

// void assertVkResult(VkResult result)
// {
//     if (result != VK_SUCCESS)
//     {
//         throw std::runtime_error(string_VkResult(result));
//     }
// }

// template<typename T>
// T assertVkbResult(vkb::Result<T> result, const char* baseMessage)
// {
//     if (result)
//     {
//         return result.value();
//     }

//     std::string errorMessage(baseMessage);
//     errorMessage.append(": ");
//     errorMessage.append(result.error().message());

//     auto failureReasons = result.detailed_failure_reasons();
//     if (failureReasons.size() != 0)
//     {
//         for (const auto& reason : failureReasons)
//         {
//             errorMessage.append("\n\t");
//             errorMessage.append(reason.c_str());
//         }
//     }

//     throw std::runtime_error(errorMessage);
// }

VkCommandPoolCreateInfo createCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = flags;
    info.queueFamilyIndex = queueFamilyIndex;
    return info;
}

VkCommandBufferAllocateInfo createCommandPoolAllocateInfo(VkCommandPool commandPool, uint32_t commandBufferCount, VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = commandPool;
    info.commandBufferCount = commandBufferCount;
    info.level = level;
    return info;
}

} // namespace VulkanUtils