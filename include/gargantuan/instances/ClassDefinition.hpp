#pragma once

#include "gargantuan/scripting/ScriptType.hpp"

#include <optional>
#include <string_view>
#include <unordered_map>

namespace gargantuan::instances {

#define DEFINE_SIMPLE_PROPERTY(classType, name, propertyType)                                                          \
    {#name, PropertyDefinition::fromSimple(#name, propertyType, &classType::name)}

class Instance;

struct PropertyDefinition {
  public:
    std::string_view Name;
    scripting::AnyScriptType Type;

    std::function<std::any(Instance *instance)> Read;
    std::function<void(Instance *instance, const std::any &value)> Write;

    template <typename ClassType, typename MemberType>
    static PropertyDefinition
    fromSimple(std::string_view name, scripting::AnyScriptType type, MemberType ClassType::*member) {
        return PropertyDefinition{
            .Name = name,
            .Type = type,
            .Read =
                [member](Instance *instance) {
                    const auto *concrete = static_cast<const ClassType *>(instance);
                    return std::any(concrete->*member);
                },
            .Write =
                [member](Instance *instance, const std::any &value) {
                    auto *concrete = static_cast<ClassType *>(instance);
                    concrete->*member = std::any_cast<MemberType>(value);
                },
        };
    };
};

struct MethodDefinition {
    // TODO
};

struct ClassDefinition {
    std::string_view Name;
    std::optional<std::string_view> Superclass;
    std::unordered_map<std::string_view, PropertyDefinition> Properties = {};
    std::unordered_map<std::string_view, PropertyDefinition> Methods = {};
};

} // namespace gargantuan::instances
