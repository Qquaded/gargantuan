#pragma once

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/render/Meshes.hpp"

#include <glm/glm.hpp>

namespace gargantuan::instances {

class BasePart : public Instance {
  public:
    const char *CLASS_NAME = "BasePart";

    glm::vec3 Size = glm::vec3(2, 1, 4);
    datatypes::CFrame CFrame = datatypes::CFrame();
    datatypes::Color3 Color = datatypes::Color3();
    float Transparency = 0.0;

    virtual render::Mesh GetMesh() const = 0;
};

} // namespace gargantuan::instances
