#pragma once

#include "gargantuan/Types.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gargantuan::instances {

class Instance;

namespace internal {
template <typename MemberType> void WriteProperty(MemberType &member, const std::any &value) {
    if constexpr (std::is_same_v<MemberType, std::shared_ptr<gargantuan::instances::Instance>>) {
        if (const auto *sharedPtr = std::any_cast<std::shared_ptr<gargantuan::instances::Instance>>(&value)) {
            member = *sharedPtr;
        } else if (auto *rawPtr =
                       std::any_cast<std::enable_shared_from_this<gargantuan::instances::Instance> *>(&value)) {
            member = *rawPtr ? (*rawPtr)->shared_from_this() : nullptr;
        }
    } else {
        if (const auto *valPtr = std::any_cast<MemberType>(&value)) {
            member = *valPtr;
        }
    }
}
} // namespace internal

#define READONLY_PROPERTY_DEFINITION(classType, name, propertyType)                                                    \
    PropertyDefinition {                                                                                               \
        .Name = #name, .Type = propertyType, .Read = [](Instance *instance) {                                          \
            const auto *concrete = static_cast<const classType *>(instance);                                           \
            return std::any(concrete->*(&classType::name));                                                            \
        },                                                                                                             \
    }

#define WRITEONLY_PROPERTY_DEFINITION(classType, name, propertyType)                                                   \
    PropertyDefinition {                                                                                               \
        .Name = #name, .Type = propertyType, .Write = [](Instance *instance, const std::any &value) {                  \
            auto *concrete = static_cast<classType *>(instance);                                                       \
            internal::WriteProperty(concrete->*(&classType::name), value);                                             \
        },                                                                                                             \
    }

#define READWRITE_PROPERTY_DEFINITION(classType, name, propertyType)                                                   \
    PropertyDefinition {                                                                                               \
        .Name = #name, .Type = propertyType,                                                                           \
        .Read =                                                                                                        \
            [](Instance *instance) {                                                                                   \
                const auto *concrete = static_cast<const classType *>(instance);                                       \
                return std::any(concrete->*(&classType::name));                                                        \
            },                                                                                                         \
        .Write = [](Instance *instance, const std::any &value) {                                                       \
            auto *concrete = static_cast<classType *>(instance);                                                       \
            internal::WriteProperty(concrete->*(&classType::name), value);                                             \
        },                                                                                                             \
    }

#define READONLY_PROPERTY_PAIR(classType, name, propertyType)                                                          \
    {#name, READONLY_PROPERTY_DEFINITION(classType, name, propertyType)}

#define WRITEONLY_PROPERTY_PAIR(classType, name, propertyType)                                                         \
    {#name, WRITEONLY_PROPERTY_DEFINITION(classType, name, propertyType)}

#define READWRITE_PROPERTY_PAIR(classType, name, propertyType)                                                         \
    {#name, READWRITE_PROPERTY_DEFINITION(classType, name, propertyType)}

struct PropertyDefinition {
  public:
    std::string_view Name;
    Types::AnyType Type;

    std::function<std::any(Instance *instance)> Read;
    std::function<void(Instance *instance, const std::any &value)> Write;
};

struct MethodArgument {
    std::string_view Name;
    Types::AnyType Type;
};

struct MethodReturn {
    Types::AnyType Type;
};

struct MethodDefinition {
    std::vector<MethodArgument> Arguments = {};
    std::vector<MethodReturn> Returns = {};
    std::function<std::vector<std::any>(Instance *instance, std::vector<std::any> arguments)> Invoke;
};

struct ClassDefinition {
    std::string_view Name;
    std::optional<std::string_view> Superclass;

    std::function<std::shared_ptr<Instance>()> Constructor;
    template <typename T> static std::function<std::shared_ptr<Instance>()> WrapConstructor() {
        return []() -> std::shared_ptr<Instance> { return std::make_shared<T>(); };
    }

    std::unordered_map<std::string_view, PropertyDefinition> Properties = {};
    std::unordered_map<std::string_view, MethodDefinition> Methods = {};
};

} // namespace gargantuan::instances
