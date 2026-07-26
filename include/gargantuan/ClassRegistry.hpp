#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <string_view>
#include <typeindex>
#include <vector>

namespace gargantuan::ClassRegistry {
	std::unordered_map<std::type_index, Instance::ClassDefinition> &GetDefinitionsMap();

	template <typename T> Instance::ClassDefinition *GetDefinition() {
		auto map = GetDefinitionsMap();
		auto it = map.find(std::type_index(typeid(T)));
		if (it != map.end()) {
			return &it->second;
		}
		return nullptr;
	}

	Instance::ClassDefinition *GetDefinition(Instance *instance);
	Instance::ClassDefinition *GetDefinitionByName(std::string_view name);
	std::vector<std::string_view> GetClassNames();
} // namespace gargantuan::ClassRegistry
