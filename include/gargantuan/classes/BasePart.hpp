#pragma once

#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/MeshProvider.hpp"

#include <glm/glm.hpp>

namespace gargantuan {

class BasePart : public Instance {
  public:
    static const ClassDefinition DEFINITION;

    bool Anchored = false;
    CFrame CFrame;
    Color3 Color;
    glm::vec3 Size = glm::vec3(2, 1, 4);
    float Transparency = 0.0f;

    virtual std::unique_ptr<GpuMesh> &GetMesh(MeshProvider &provider) const = 0;
};

} // namespace gargantuan
