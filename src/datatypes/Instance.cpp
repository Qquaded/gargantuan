#include "gargantuan/datatypes/Instance.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>

namespace gargantuan::datatypes {

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

        auto it = std::find_if(oldChildren.begin(), lastChild,
                               [this](const std::shared_ptr<Instance> &child) { return child.get() == this; });

        if (it != lastChild) {
            oldChildren.erase(it);
        }
    }

    Parent = newParentInstance;

    if (Parent != nullptr) {
        Parent->Children.push_back(shared_from_this());
    }
}

std::vector<std::shared_ptr<Instance>> Instance::GetDescendants() {
    std::vector<std::shared_ptr<Instance>> descendants;
    CollectDescendants(descendants);
    return descendants;
}

void Instance::CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants) {
    for (const auto &child : Children) {
        descendants.push_back(child);
        child->CollectDescendants(descendants);
    }
}

} // namespace gargantuan::datatypes
