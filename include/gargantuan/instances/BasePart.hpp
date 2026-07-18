#pragma once

#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/Meshes.hpp"

#include <glm/glm.hpp>

namespace gargantuan::instances {

class BasePart : public gargantuan::datatypes::Instance {
  public:
    const char *CLASS_NAME = "BasePart";

    glm::vec3 Position = glm::vec3(0, 0, 0);
    glm::vec3 Color = glm::vec3(0, 0, 0);
    float Transparency = 0.0;

    virtual render::Mesh GetMesh() const = 0;
};

} // namespace gargantuan::instances
