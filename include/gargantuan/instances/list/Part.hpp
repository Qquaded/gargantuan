#pragma once

#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/instances/list/BasePart.hpp"

namespace gargantuan::instances {

enum class Shape { Cube, Wedge, Sphere, Cylinder };

class Part : public BasePart {
  public:
    static const ClassDefinition DEFINITION;

    Shape Shape = Shape::Cube;

    std::unique_ptr<render::GpuMesh> RenderMesh = nullptr;

    render::Mesh GetMesh() const override;

    void UploadGeometry(SDL_GPUDevice *gpu) { RenderMesh = std::make_unique<render::GpuMesh>(gpu, GetMesh()); }
};

} // namespace gargantuan::instances
