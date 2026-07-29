#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <SDL3/SDL_log.h>
#include <cstddef>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace gargantuan::InstanceClassRegistry {
	std::unordered_map<std::type_index, InstanceClassDefinition> &GetDefinitionsMap() {
		static std::unordered_map<std::type_index, InstanceClassDefinition> definitions;
		return definitions;
	}

	namespace {
		// Name to entry. A superclass is named rather than pointed at, so
		// resolving one used to be a scan of every registered class comparing
		// strings -- once per level, on every property read and write.
		std::unordered_map<std::string_view, InstanceClassDefinition *> &GetNameIndex() {
			static std::unordered_map<std::string_view, InstanceClassDefinition *> index;
			return index;
		}

		bool NameIndexBuilt = false;

		void EnsureNameIndex() {
			if (NameIndexBuilt) {
				return;
			}

			auto &index = GetNameIndex();
			index.clear();
			for (auto &entry : GetDefinitionsMap()) {
				index.emplace(entry.second.ClassName, &entry.second);
			}
			NameIndexBuilt = true;
		}

		// Walks the chain once and copies what it finds into the class it
		// started from. Nearest first, and emplace leaves an existing entry
		// alone, so a class that redeclares an inherited member still shadows
		// it -- which is what the walk this replaces did by stopping at its
		// first hit.
		void Flatten(InstanceClassDefinition *definition) {
			definition->AllProperties.clear();
			definition->AllMethods.clear();

			for (InstanceClassDefinition *current = definition; current;) {
				for (auto &[name, property] : current->Properties) {
					definition->AllProperties.emplace(name, &property);
				}
				for (auto &[name, method] : current->Methods) {
					definition->AllMethods.emplace(name, &method);
				}

				if (!current->Superclass.has_value()) {
					break;
				}

				// A class whose superclass names itself would spin here
				InstanceClassDefinition *next = GetDefinitionByName(current->Superclass.value());
				if (next == current) {
					break;
				}
				current = next;
			}

			definition->Flattened = true;
		}
	}

	void InvalidateCaches() {
		NameIndexBuilt = false;
		GetNameIndex().clear();
		for (auto &entry : GetDefinitionsMap()) {
			entry.second.Flattened = false;
			entry.second.AllProperties.clear();
			entry.second.AllMethods.clear();
		}
	}

	InstanceClassDefinition *GetDefinitionByType(std::type_index type) {
		// NOTE: a reference -- copying the map would hand back a pointer into a
		// temporary that dies with this call
		auto &map = GetDefinitionsMap();
		auto it = map.find(type);
		if (it == map.end()) {
			return nullptr;
		}

		InstanceClassDefinition *definition = &it->second;
		if (!definition->Flattened) {
			Flatten(definition);
		}
		return definition;
	}

	InstanceClassDefinition *GetDefinition(Instance *instance) {
		if (!instance) return nullptr;

		// Worked out once per instance: finding it costs a typeid and a hash of
		// the resulting type_index, and an instance cannot change class. The
		// tables it points at can still have been dropped since, which is what
		// the second check is for.
		if (instance->CachedDefinition) {
			if (!instance->CachedDefinition->Flattened) {
				Flatten(instance->CachedDefinition);
			}
			return instance->CachedDefinition;
		}

		instance->CachedDefinition = GetDefinitionByType(std::type_index(typeid(*instance)));
		return instance->CachedDefinition;
	};

	InstanceClassDefinition *GetDefinitionByName(std::string_view name) {
		EnsureNameIndex();
		auto &index = GetNameIndex();
		auto it = index.find(name);
		return it != index.end() ? it->second : nullptr;
	}

	std::vector<std::string_view> GetClassNames() {
		auto &map = GetDefinitionsMap();
		std::vector<std::string_view> result;
		result.reserve(map.size());
		for (auto &definition : map) {
			result.emplace_back(definition.second.ClassName);
		}
		return result;
	}
}
