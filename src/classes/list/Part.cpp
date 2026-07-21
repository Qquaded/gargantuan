#include "gargantuan/classes/list/Part.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/PrimitiveMeshes.hpp"

#include <stdexcept>

namespace gargantuan {

const Instance::ClassDefinition Part::DEFINITION = {
    .Name = "Part",
    .Superclass = "BasePart",
    .Constructor = ClassDefinition::WrapConstructor<Part>(),
    .Properties = {
        // DEFINE_SIMPLE_PROPERTY(Part, CFrame, scripting::types::CFRAME),
        // DEFINE_SIMPLE_PROPERTY(Part, Color, scripting::types::COLOR3),
        // DEFINE_SIMPLE_PROPERTY(Part, Transparency, scripting::types::FLOAT),
    },
};

Mesh Part::GetMesh() const {
    glm::vec3 color = Color;
    auto rgba = glm::vec4(color, 1.0f - Transparency);

    switch (Shape) {
    case Part::Shape::Cube:
        return PrimitiveMeshes::Cube(rgba);
    case Part::Shape::Wedge:
        return PrimitiveMeshes::Wedge(rgba);
    case Part::Shape::Sphere:
        return PrimitiveMeshes::Sphere(rgba);
    default:
        throw std::runtime_error("Unimplemented");
    }
};

} // namespace gargantuan
