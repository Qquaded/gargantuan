#pragma once

#include "gargantuan/classes/list/BasePart.hpp"
#include "gargantuan/datatypes/Instance.hpp"

namespace gargantuan {

class Part : public BasePart {
  public:
    enum class Shape { Cube, Wedge, Sphere };

    static const ClassDefinition DEFINITION;

    Shape Shape = Shape::Cube;

    std::unique_ptr<GpuMesh> RenderMesh = nullptr;

    Mesh GetMesh() const override;

    void UploadGeometry(SDL_GPUDevice *gpu) { RenderMesh = std::make_unique<GpuMesh>(gpu, GetMesh()); }
};

} // namespace gargantuan
