#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/ClassRegistry.hpp"
#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cstddef>
#include <lua.h>
#include <lualib.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace gargantuan {
	G_UD_IMPL_PRELUDE(Instance);
	G_UD_IMPL_PROPS(Instance);
	G_UD_IMPL_METHODS(Instance);

	const Instance::ClassDefinition Instance::DEFINITION = {
		.Name = "Instance",
		.Properties =
			{
				{"Name", Property::fromSimple<&Instance::Name>(true, true)},
				{
					"ClassName",
					Property::fromRead([](Instance *instance) -> std::string_view {
						return ClassRegistry::GetDefinition(instance)->Name;
					}),
				},
				{
					"Parent",
					Property::fromReadWrite<Instance::Pointer>(
						[](Instance *instance) -> std::optional<Instance::Pointer> {
							return instance->Parent ? instance->Parent->shared_from_this() : nullptr;
						},
						[](Instance *instance, Instance::Pointer newParent) { instance->SetParent(newParent); }
					),
				},
			},
		.Methods = {
			{"IsA", Method::Wrap<&Instance::IsA>()},
			{"GetFullName", Method::Wrap<&Instance::GetFullName>()},
			{"GetChildren", Method::Wrap<&Instance::GetChildren>()},
			{"GetDescendants", Method::Wrap<&Instance::GetDescendants>()},
			{"FindFirstChild", Method::Wrap<&Instance::FindFirstChild>()},
			{"FindFirstChildOfClass", Method::Wrap<&Instance::FindFirstChildOfClass>()},
		}
	};

	// TODO: fire DescendantAdded/Removed signals
	void Instance::SetParent(std::shared_ptr<Instance> newParent) {
		std::shared_ptr<Instance> self = shared_from_this();

		if (Parent != nullptr) {
			auto &oldChildren = Parent->Children;
			if (auto it = std::find(oldChildren.begin(), oldChildren.end(), self); it != oldChildren.end()) {
				oldChildren.erase(it);
				Parent->ChildRemoved->Fire(self);
			}
		}

		Parent = newParent.get();

		if (newParent != nullptr) {
			newParent->Children.push_back(self);
			newParent->ChildAdded->Fire(self);
		}
	}

	std::optional<Instance::Userdata::Property> Instance::FindProperty(std::string_view name) {
		auto currentDefinition = ClassRegistry::GetDefinition(this);
		while (currentDefinition) {
			if (auto it = currentDefinition->Properties.find(name); it != currentDefinition->Properties.end()) {
				return it->second;
			}

			auto superclass = currentDefinition->Superclass;
			if (superclass.has_value()) {
				currentDefinition = ClassRegistry::GetDefinitionByName(superclass.value());
				continue;
			} else {
				return {};
			}
		}
		return {};
	}

	std::optional<Instance::Userdata::Method> Instance::FindMethod(std::string_view name) {
		auto currentDefinition = ClassRegistry::GetDefinition(this);
		while (currentDefinition) {
			if (auto it = currentDefinition->Methods.find(name); it != currentDefinition->Methods.end()) {
				return it->second;
			}

			auto superclass = currentDefinition->Superclass;
			if (superclass.has_value()) {
				currentDefinition = ClassRegistry::GetDefinitionByName(superclass.value());
				continue;
			} else {
				return {};
			}
		}
		return {};
	}

	int Instance::UserdataIndex(lua_State *L) {
		Instance::Pointer instance = CheckStackValue<Instance::Pointer>(L, 1);
		const char *key = luaL_checkstring(L, 2);

		if (key && instance) {
			auto property = instance->FindProperty(key);
			if (property.has_value()) {
				if (property->Read) {
					lua_remove(L, 1);
					lua_remove(L, 1);
					return property->PushStack(L, property->Read(instance.get()));
				} else {
					luaL_error(L, "Property %s is write-only", key);
				}
			} else if (auto child = instance->FindFirstChild(key)) {
				lua_settop(L, 0);
				StackValue<Instance::Pointer>::Push(L, child);
				return 1;
			}
		}

		return 0;
	};

	int Instance::UserdataNewIndex(lua_State *L) {
		Instance::Pointer instance = CheckStackValue<Instance::Pointer>(L, 1);
		const char *key = luaL_checkstring(L, 2);

		if (key && instance) {
			auto property = instance->FindProperty(key);
			if (property.has_value()) {
				if (property->Write) {
					auto value = property->CheckStack(L, 3);
					property->Write(instance.get(), value);
					return 0;
				} else {
					luaL_error(L, "Property %s is read-only", key);
				}
			}
		}

		luaL_error(L, "Unknown property %s", key);

		return 0;
	};

	int Instance::UserdataNamecall(lua_State *L) {
		Instance::Pointer instance = CheckStackValue<Instance::Pointer>(L, 1);
		const char *key = lua_namecallatom(L, nullptr);

		if (key && instance) {
			auto method = instance->FindMethod(key);
			if (method.has_value()) {
				return method->Call(L, instance.get());
			}
		}

		luaL_error(L, "%s is not a valid method of %s", key, instance->Name.data());
		return 0;
	};

	std::string Instance::GetFullName() {
		std::vector<std::string_view> path;

		size_t totalLength = 0;
		Instance *current = this;

		while (current) {
			auto &name = current->Name;
			path.push_back(name);
			totalLength += name.size() + 1;
			current = current->Parent;
		};

		if (path.empty()) {
			return "";
		}

		if (totalLength > 0) {
			totalLength--;
		}

		std::string fullName;
		fullName.reserve(totalLength);

		auto begin = path.rbegin();
		for (auto it = begin; it != path.rend(); ++it) {
			if (it != begin) {
				fullName.push_back('.');
			}
			fullName.append(*it);
		}

		return fullName;
	};

	bool Instance::IsA(std::string_view className) {
		auto currentDefinition = ClassRegistry::GetDefinition(this);
		while (true) {
			if (currentDefinition->Name == className) {
				return true;
			}

			auto superclass = currentDefinition->Superclass;
			if (superclass.has_value()) {
				currentDefinition = ClassRegistry::GetDefinitionByName(superclass.value());
			} else {
				return false;
			}
		}
	}

	std::vector<std::shared_ptr<Instance>> &Instance::GetChildren() {
		return Children;
	}

	void Instance::CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants) {
		for (const auto &child : Children) {
			descendants.push_back(child);
			child->CollectDescendants(descendants);
		}
	}

	std::vector<std::shared_ptr<Instance>> Instance::GetDescendants() {
		std::vector<std::shared_ptr<Instance>> descendants;
		CollectDescendants(descendants);
		return descendants;
	}

	std::shared_ptr<Instance> Instance::FindFirstChild(std::string_view name, bool recursive) {
		for (const auto &child : Children) {
			if (child->Name == name) {
				return child;
			}
		};
		return nullptr;
	}

	std::shared_ptr<Instance> Instance::FindFirstChildOfClass(std::string_view className) {
		for (const auto &child : Children) {
			if (ClassRegistry::GetDefinition(child.get())->Name == className) {
				return child;
			}
		};
		return nullptr;
	}
} // namespace gargantuan
