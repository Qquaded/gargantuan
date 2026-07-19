#include "gargantuan/instances/list/Part.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/render/PrimitiveMeshes.hpp"
#include "gargantuan/scripting/ScriptType.hpp"

namespace gargantuan::instances {

const ClassDefinition Part::DEFINITION = {
    .Name = "Part",
    .Superclass = "BasePart",
    .Properties = {
        // DEFINE_SIMPLE_PROPERTY(Part, CFrame, scripting::TYPE_CFRAME),
        // DEFINE_SIMPLE_PROPERTY(Part, Color, scripting::TYPE_COLOR3),
        DEFINE_SIMPLE_PROPERTY(Part, Transparency, scripting::TYPE_FLOAT),
    }
};

render::Mesh Part::GetMesh() const {
    glm::vec3 color = Color;
    auto rgba = glm::vec4(color, Transparency);

    switch (Shape) {
    case gargantuan::instances::Shape::Block:
        return render::PrimitiveMeshes::Block(rgba);
    }
};

} // namespace gargantuan::instances
