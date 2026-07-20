#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/Types.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/instances/ClassRegistry.hpp"

#include <algorithm>
#include <any>
#include <cstddef>
#include <memory>
#include <objc/objc.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace gargantuan::instances {

const ClassDefinition Instance::DEFINITION = {
    .Name = "Instance",
    .Properties =
        {
            READWRITE_PROPERTY_PAIR(Instance, Name, Types::STRING),
            {
                "ClassName",
                PropertyDefinition{
                    .Name = "ClassName",
                    .Type = Types::STRING,
                    .Read = [](Instance *instance) -> std::any { return ClassRegistry::GetDefinition(instance)->Name; },
                },
            },
            {
                "Parent",
                PropertyDefinition{
                    .Name = "Parent",
                    .Type = Types::INSTANCE,
                    .Read = [](Instance *instance) -> std::any {
                        if (auto parent = instance->Parent) {
                            return parent->shared_from_this();
                        };
                        return nullptr;
                    },
                    .Write = [](Instance *instance, const std::any &value) -> void {
                        InstancePointer newParent = std::any_cast<InstancePointer>(value);
                        instance->SetParent(newParent);
                    },
                },
            },
        },
    .Methods = {
        {
            "IsA",
            MethodDefinition{
                .Arguments = {MethodArgument{.Name = "className", .Type = Types::STRING}},
                .Returns = {MethodReturn{.Type = Types::BOOLEAN}},
                .Invoke = [](Instance *instance, std::vector<std::any> arguments) -> std::vector<std::any> {
                    auto className = std::any_cast<std::string_view>(arguments[0]);
                    auto currentDefinition = ClassRegistry::GetDefinition(instance);
                    while (true) {
                        if (currentDefinition->Name == className) {
                            return {true};
                        }

                        auto superclass = currentDefinition->Superclass;
                        if (superclass.has_value()) {
                            currentDefinition = ClassRegistry::GetDefinitionByName(superclass.value());
                        } else {
                            return {false};
                        }
                    }
                },
            },
        },
        {
            "GetChildren",
            MethodDefinition{
                .Returns = {MethodReturn{.Type = Types::Array(Types::INSTANCE)}},
                .Invoke = [](Instance *instance, std::vector<std::any> arguments) -> std::vector<std::any> {
                    std::vector<Instance *> result;
                    result.reserve(instance->Children.size());
                    for (auto &instance : instance->Children) {
                        result.emplace_back(instance.get());
                    }
                    return {result};
                },
            },
        },
        {
            "GetDescendants",
            MethodDefinition{
                .Returns = {MethodReturn{.Type = Types::Array(Types::INSTANCE)}},
                .Invoke = [](Instance *instance, std::vector<std::any> arguments) -> std::vector<std::any> {
                    auto descendants = instance->GetDescendants();
                    std::vector<Instance *> result;
                    result.reserve(descendants.size());
                    for (auto &instance : descendants) {
                        result.emplace_back(instance.get());
                    }
                    return {result};
                },
            },
        },
        {
            "GetFullName",
            MethodDefinition{
                .Returns = {MethodReturn{.Type = Types::STRING}},
                .Invoke = [](Instance *instance, std::vector<std::any> arguments) -> std::vector<std::any> {
                    return {instance->GetFullName()};
                },
            },
        },
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

std::string Instance::GetFullName() {
    std::vector<std::string_view> path;

    // Start from -1 to omit a trailing period
    size_t totalLength = 0;
    instances::Instance *current = this;

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
