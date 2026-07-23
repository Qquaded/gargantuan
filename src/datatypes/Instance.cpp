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
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace gargantuan {

template <> UserdataTag Instance::This::GetUserdataTag() { return UserdataTag::Instance; };
template <> std::string_view Instance::This::GetUserdataType() { return "Instance"; };

const Instance::ClassDefinition Instance::DEFINITION = {
    .Name = "Instance",
    .Properties =
        {
            USERDATA_READWRITE_PROP(Instance, Name, std::string_view),
            {
                "ClassName",
                {
                    +[](lua_State *L, Instance *instance) -> int {
                        StackValue<std::string_view>::Push(L, ClassRegistry::GetDefinition(instance)->Name);
                        return 1;
                    },
                    nullptr,
                },
            },
            {
                "Parent",
                {
                    +[](lua_State *L, Instance *instance) -> int {
                        if (auto parent = instance->Parent) {
                            StackValue<Instance::Userdata>::Push(L, parent->shared_from_this());
                        } else {
                            lua_pushnil(L);
                        };
                        return 1;
                    },
                    +[](lua_State *L, Instance *instance) -> int {
                        Instance::Pointer newParent = CheckStackValue<Instance::Pointer>(L, -1);
                        instance->SetParent(newParent);
                        return 0;
                    },
                },
            },
        },
    .Methods = {
        {"IsA", Method::Wrap<&Instance::GetFullName>()},
        {"GetFullName", Method::Wrap<&Instance::GetFullName>()},
        {"GetChildren", Method::Wrap<&Instance::GetChildren>()},
        {"GetDescendants", Method::Wrap<&Instance::GetDescendants>()},
        {"FindFirstChild", Method::Wrap<&Instance::FindFirstChild>()},
        {"FindFirstChildOfClass", Method::Wrap<&Instance::FindFirstChildOfClass>()},
    }
};

void Instance::SetParent(std::shared_ptr<Instance> newParent) {
    auto newParentInstance = newParent.get();

    if (Parent == newParentInstance) {
        return;
    }

    if (newParentInstance == this) {
        throw std::runtime_error("Cannot set parent to itself");
    }

    std::shared_ptr<Instance> self = weak_from_this().lock();
    if (!self) {
        throw std::runtime_error("Instance must be managed by std::shared_ptr before setting a parent");
    }

    if (Parent != nullptr) {
        auto &oldChildren = Parent->Children;
        auto it = std::find_if(oldChildren.begin(), oldChildren.end(), [this](const std::shared_ptr<Instance> &child) {
            return child.get() == this;
        });

        if (it != oldChildren.end()) {
            oldChildren.erase(it);
        }
    }

    Parent = newParentInstance;

    if (Parent != nullptr) {
        Parent->Children.emplace_back(self);
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
    Instance::Pointer instance = StackValue<Instance::Pointer>::From(L, 1);
    const char *key = luaL_checkstring(L, 2);

    if (key && instance) {
        auto property = instance->FindProperty(key);
        if (property.has_value()) {
            if (property->Read) {
                lua_remove(L, 1);
                lua_remove(L, 1);
                property->Read(L, instance.get());
                return 1;
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
    Instance::Pointer instance = StackValue<Instance::Pointer>::From(L, 1);
    const char *key = luaL_checkstring(L, 2);

    if (key && instance) {
        auto property = instance->FindProperty(key);
        if (property.has_value()) {
            if (property->Write) {
                return property->Write(L, instance.get());
            } else {
                luaL_error(L, "Property %s is read-only", key);
            }
        }
    }

    luaL_error(L, "Unknown property %s", key);

    return 0;
};

int Instance::UserdataNamecall(lua_State *L) {
    Instance::Pointer instance = StackValue<Instance::Pointer>::From(L, 1);
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

    // Start from -1 to omit a trailing period
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

std::vector<std::shared_ptr<Instance>> Instance::GetChildren() { return Children; }

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
