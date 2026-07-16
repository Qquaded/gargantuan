#pragma once

#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <VkBootstrap.h>

inline void assertVkResult(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        spdlog::error(string_VkResult(result));
        abort();
    }
};

template<typename T>
inline T assertVkbResult(vkb::Result<T> result, const char* baseMessage)
{
    if (result)
    {
        return result.value();
    }

    std::string errorMessage(baseMessage);
    errorMessage.append(": ");
    errorMessage.append(result.error().message());

    auto failureReasons = result.detailed_failure_reasons();
    if (failureReasons.size() != 0)
    {
        for (const auto& reason : failureReasons)
        {
            errorMessage.append("\n\t");
            errorMessage.append(reason.c_str());
        }
    }

    spdlog::error(errorMessage);
    abort();
};