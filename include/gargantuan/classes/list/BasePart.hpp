#pragma once

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/Meshes.hpp"

#include <glm/glm.hpp>

namespace gargantuan {

class BasePart : public Instance {
  public:
    static const ClassDefinition DEFINITION;

    glm::vec3 Size = glm::vec3(2, 1, 4);
    CFrame CFrame;
    Color3 Color;
    float Transparency = 0.0f;

    virtual Mesh GetMesh() const = 0;
};

} // namespace gargantuan
