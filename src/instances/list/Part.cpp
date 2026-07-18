#include "gargantuan/instances/list/Part.hpp"
#include "gargantuan/render/PrimitiveMeshes.hpp"

namespace gargantuan::instances {

render::Mesh Part::GetMesh() const {
    glm::vec3 color = Color;
    auto rgba = glm::vec4(color, Transparency);

    switch (Shape) {
    case gargantuan::instances::Shape::Block:
        return render::PrimitiveMeshes::Block(rgba);
    }
};

} // namespace gargantuan::instances
