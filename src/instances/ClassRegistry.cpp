#include "gargantuan/instances/ClassRegistry.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/instances/list/BasePart.hpp"
#include "gargantuan/instances/list/DataModel.hpp"
#include "gargantuan/instances/list/Part.hpp"

#include <SDL3/SDL_log.h>
#include <cstddef>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

#define USE_INSTANCE_DEFINITION(instance) {typeid(instance), instance::DEFINITION}

namespace gargantuan::instances::ClassRegistry {

std::unordered_map<std::type_index, ClassDefinition> &GetDefinitionsMap() {
    static auto *CLASS_DEFINITIONS = new std::unordered_map<std::type_index, ClassDefinition>{
        USE_INSTANCE_DEFINITION(BasePart),
        USE_INSTANCE_DEFINITION(DataModel),
        USE_INSTANCE_DEFINITION(Instance),
        USE_INSTANCE_DEFINITION(Part),
    };
    return *CLASS_DEFINITIONS;
}

ClassDefinition *GetDefinition(Instance *instance) {
    if (!instance)
        return nullptr;
    auto &map = GetDefinitionsMap();
    SDL_Log("???: %s", typeid(*instance).name());
    auto it = map.find(std::type_index(typeid(*instance)));
    if (it != map.end()) {
        return &it->second;
    }
    return nullptr;
};

ClassDefinition *GetDefinitionByName(std::string_view name) {
    auto &map = GetDefinitionsMap();
    for (auto &definition : map) {
        if (definition.second.Name == name) {
            return &definition.second;
        }
    }
    return nullptr;
}

std::vector<std::string_view> GetClassNames() {
    auto &map = GetDefinitionsMap();
    std::vector<std::string_view> result;
    result.reserve(map.size());
    for (auto &definition : map) {
        result.emplace_back(definition.second.Name);
    }
    return result;
}

void CollectProperties(ClassDefinition *definition, std::unordered_map<std::string_view, PropertyDefinition> &props) {
    for (auto property : definition->Properties) {
        props.emplace(property);
    }

    auto superclass = definition->Superclass;
    if (superclass) {
        auto superclassDefinition = GetDefinitionByName(superclass.value());
        if (superclassDefinition) {
            CollectProperties(superclassDefinition, props);
        }
    }
}

std::unordered_map<std::string_view, PropertyDefinition> GetProperties(ClassDefinition *definition) {
    std::unordered_map<std::string_view, PropertyDefinition> props;
    CollectProperties(definition, props);
    return props;
}

} // namespace gargantuan::instances::ClassRegistry
