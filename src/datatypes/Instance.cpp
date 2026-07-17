#include "gargantuan/datatypes/Instance.hpp"

#include <algorithm>

namespace gargantuan::datatypes {

void Instance::SetParent(Instance *newParent) {
    if (Parent == newParent) {
        return;
    }

    if (Parent) {
        auto &children = Parent->Children;
        children.erase(
            std::remove_if(children.begin(), children.end(), [this](const auto &ptr) { return ptr.get() == this; }),
            children.end());
    }

    Parent = newParent;

    // TODO: add to newparent
}
} // namespace gargantuan::datatypes
