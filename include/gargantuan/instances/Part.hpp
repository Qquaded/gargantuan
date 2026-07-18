#pragma once

#include "gargantuan/instances/BasePart.hpp"

namespace gargantuan::instances {

enum class Shape { Block };

class Part : public BasePart {
    const char *CLASS_NAME = "Part";

  public:
    Shape Shape = Shape::Block;

    std::unique_ptr<render::GpuMesh> RenderMesh = nullptr;

    render::Mesh GetMesh() const override;

    void UploadGeometry(SDL_GPUDevice *gpu) { RenderMesh = std::make_unique<render::GpuMesh>(gpu, GetMesh()); }
};

} // namespace gargantuan::instances
