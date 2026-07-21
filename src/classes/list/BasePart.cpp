#include "gargantuan/classes/list/BasePart.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {

const Instance::ClassDefinition BasePart::DEFINITION = {
    .Name = "BasePart",
    .Superclass = "Instance",
    .Properties = {
        // USERDATA_READWRITE_PROP(BasePart, CFrame, CFrame),
        // USERDATA_READWRITE_PROP(BasePart, Color, Color3),
        // USERDATA_READWRITE_PROP(BasePart, Transparency, float),
    }
};
} // namespace gargantuan
