#include "gargantuan/instances/list/BasePart.hpp"
#include "gargantuan/Types.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"

namespace gargantuan::instances {

const ClassDefinition BasePart::DEFINITION = {
    .Name = "BasePart",
    .Superclass = "Instance",
    .Properties = {
        // DEFINE_SIMPLE_PROPERTY(BasePart, CFrame, scripting::types::CFRAME),
        // DEFINE_SIMPLE_PROPERTY(BasePart, Color, scripting::types::COLOR3),
        READWRITE_PROPERTY_PAIR(BasePart, Transparency, Types::FLOAT),
    }
};
} // namespace gargantuan::instances
