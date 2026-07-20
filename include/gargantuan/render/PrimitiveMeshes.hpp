#pragma once

#include "gargantuan/render/Meshes.hpp"

#include <ext/vector_float4.hpp>

namespace gargantuan::render::PrimitiveMeshes {

Mesh Cube(glm::vec4 rgba);
Mesh Wedge(glm::vec4 rgba);
Mesh Sphere(glm::vec4 rgba);
Mesh Cylinder(glm::vec4 rgba);

} // namespace gargantuan::render::PrimitiveMeshes
