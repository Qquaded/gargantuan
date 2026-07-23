#include "gargantuan/services/Workspace.hpp"

namespace gargantuan {

const Workspace::ClassDefinition Workspace::DEFINITION = {
    .Name = "Workspace",
    .Constructor = ClassDefinition::WrapConstructor<Workspace>(),
};

}
