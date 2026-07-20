#include "gargantuan/instances/list/Part.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/render/PrimitiveMeshes.hpp"

namespace gargantuan::instances {

const ClassDefinition Part::DEFINITION = {
    .Name = "Part",
    .Superclass = "BasePart",
    .Constructor = ClassDefinition::WrapConstructor<Part>(),
    .Properties = {
        // DEFINE_SIMPLE_PROPERTY(Part, CFrame, scripting::types::CFRAME),
        // DEFINE_SIMPLE_PROPERTY(Part, Color, scripting::types::COLOR3),
        // DEFINE_SIMPLE_PROPERTY(Part, Transparency, scripting::types::FLOAT),
    },
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
