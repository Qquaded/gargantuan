#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/instances/ClassRegistry.hpp"
#include "gargantuan/scripting/ScriptType.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace gargantuan::instances {

const ClassDefinition Instance::DEFINITION = {
    .Name = "Instance",
    .Properties = {
        DEFINE_SIMPLE_PROPERTY(Instance, Name, scripting::TYPE_STRING),
        {
            "ClassName",
            PropertyDefinition{
                .Name = "ClassName",
                .Type = scripting::TYPE_STRING,
                .Read = [](Instance *instance) -> std::any { return ClassRegistry::GetDefinition(instance)->Name; },
            },
        },
        {
            "Parent",
            PropertyDefinition{
                .Name = "Parent",
                .Type = scripting::TYPE_INSTANCE,
                .Read = [](Instance *instance) -> std::any { return instance->Parent; },
                .Write = [](Instance *instance, const std::any &value) -> void {
                    auto newParent = std::any_cast<Instance *>(value);
                    instance->SetParent(newParent->shared_from_this());
                },
            },
        }
    }
};

void Instance::SetParent(std::shared_ptr<Instance> newParent) {
    auto newParentInstance = newParent.get();

    if (Parent == newParentInstance) {
        return;
    }

    if (Parent == this) {
        throw std::runtime_error("Cannot set parent to itself");
    }

    if (Parent != nullptr) {
        auto &oldChildren = Parent->Children;
        auto lastChild = oldChildren.end();

        auto it = std::find_if(oldChildren.begin(), lastChild, [this](const std::shared_ptr<Instance> &child) {
            return child.get() == this;
        });

        if (it != lastChild) {
            oldChildren.erase(it);
        }
    }

    Parent = newParentInstance;

    if (Parent != nullptr) {
        Parent->Children.push_back(shared_from_this());
    }
}

std::string Instance::GetFullName() {
    std::vector<std::string_view> path;
    // Start from -1 to omit a trailing period
    size_t totalLength = -1;
    instances::Instance *current = this->Parent;

    while (current) {
        auto &name = current->Name;
        path.push_back(name);
        totalLength += name.size() + 1;
        current = current->Parent;
    };

    if (path.empty()) {
        return "";
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

} // namespace gargantuan::instances
