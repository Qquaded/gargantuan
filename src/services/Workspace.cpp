#include "gargantuan/services/Workspace.hpp"

namespace gargantuan {

const Workspace::ClassDefinition Workspace::DEFINITION = {
    .Name = "Workspace",
    .Superclass = "WorldRoot",
    .Constructor = ClassDefinition::WrapConstructor<Workspace>(),
};

}
