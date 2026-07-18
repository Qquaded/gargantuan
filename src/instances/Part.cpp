#include "gargantuan/instances/Part.hpp"
#include "gargantuan/render/PrimitiveMeshes.hpp"

namespace gargantuan::instances {

render::Mesh Part::GetMesh() {
    auto rgba = glm::vec4(Color, Transparency);

    switch (Shape) {
    case gargantuan::instances::Shape::Block:
        return render::PrimitiveMeshes::Block(rgba);
    }
};

} // namespace gargantuan::instances
