#include "gargantuan/instances/list/BasePart.hpp"
#include "gargantuan/scripting/ScriptType.hpp"

namespace gargantuan::instances {

const ClassDefinition BasePart::DEFINITION = {
    .Name = "Part",
    .Superclass = "Instance",
    .Properties = {
        // DEFINE_SIMPLE_PROPERTY(BasePart, CFrame, scripting::TYPE_CFRAME),
        // DEFINE_SIMPLE_PROPERTY(BasePart, Color, scripting::TYPE_COLOR3),
        DEFINE_SIMPLE_PROPERTY(BasePart, Transparency, scripting::TYPE_FLOAT),
    }
};
} // namespace gargantuan::instances
