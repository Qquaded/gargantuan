#pragma once

#include "gargantuan/instances/ClassDefinition.hpp"

#include <string_view>
#include <typeindex>
#include <vector>

namespace gargantuan::instances::ClassRegistry {

std::unordered_map<std::type_index, ClassDefinition> &GetDefinitionsMap();

template <typename T> ClassDefinition *GetDefinition() {
    auto map = GetDefinitionsMap();
    auto it = map.find(std::type_index(typeid(T)));
    if (it != map.end()) {
        return &it->second;
    }
    return nullptr;
}

ClassDefinition *GetDefinition(Instance *instance);
ClassDefinition *GetDefinitionByName(std::string_view name);
std::vector<std::string_view> GetClassNames();
void CollectProperties(ClassDefinition *definition, std::unordered_map<std::string_view, PropertyDefinition> &props);
std::unordered_map<std::string_view, PropertyDefinition> GetProperties(ClassDefinition *definition);

} // namespace gargantuan::instances::ClassRegistry
