#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/classes/BasePart.hpp"

#include <SDL3/SDL_log.h>
#include <memory>

namespace gargantuan {

const WorldRoot::ClassDefinition WorldRoot::DEFINITION = {
    .Name = "WorldRoot",
    .Superclass = "Instance",
};

WorldRoot::WorldRoot() {
    ChildAdded->Connect([this](Instance::Pointer instance) {
        if (instance->IsA("BasePart")) {
            std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
            this->Parts.push_back(part);
        }
    });
}

} // namespace gargantuan
