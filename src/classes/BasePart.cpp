#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {

const Instance::ClassDefinition BasePart::DEFINITION = {
    .Name = "BasePart",
    .Superclass = "Instance",
    .Properties = {
        USERDATA_READWRITE_PROP(BasePart, Anchored, bool),
        USERDATA_READWRITE_PROP(BasePart, CanCollide, bool),
        USERDATA_READWRITE_PROP(BasePart, CFrame, gargantuan::CFrame),
        USERDATA_READWRITE_PROP(BasePart, Color, gargantuan::Color3),
        USERDATA_READWRITE_PROP(BasePart, Size, glm::vec3),
        USERDATA_READWRITE_PROP(BasePart, Transparency, float),
        {
            "Position",
            {
                [](lua_State *L, Instance *instance) -> int {
                    auto part = instance->Cast<BasePart>();
                    StackValue<glm::vec3>::Push(L, part->CFrame.Position);
                    return 1;
                },
                [](lua_State *L, Instance *instance) -> int {
                    auto part = instance->Cast<BasePart>();
                    part->CFrame = gargantuan::CFrame(StackValue<glm::vec3>::From(L, -1), part->CFrame.Rotation);
                    return 0;
                },
            },
        }
    }
};
} // namespace gargantuan
