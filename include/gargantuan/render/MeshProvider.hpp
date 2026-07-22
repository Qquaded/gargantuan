#pragma once

#include "gargantuan/render/GpuMesh.hpp"
#include "gargantuan/render/PrimitiveMeshes.hpp"

#include <string>
#include <unordered_map>

namespace gargantuan {

class MeshProvider {
  public:
    MeshProvider(SDL_GPUDevice *gpu);

    MeshProvider(const MeshProvider &) = delete;
    MeshProvider &operator=(const MeshProvider &) = delete;

    MeshProvider(MeshProvider &&) = default;
    MeshProvider &operator=(MeshProvider &&) = default;

    std::unordered_map<std::string, Mesh> UnloadedMeshes = {
        {"gargantuan://meshes/Ball", PrimitiveMeshes::Block()},
        {"gargantuan://meshes/Block", PrimitiveMeshes::Block()},
        {"gargantuan://meshes/Cylinder", PrimitiveMeshes::Block()},
        {"gargantuan://meshes/Wedge", PrimitiveMeshes::Wedge()},
        {"gargantuan://meshes/CornerWedge", PrimitiveMeshes::Block()},
    };

    std::unordered_map<std::string, std::unique_ptr<GpuMesh>> GpuMeshes;
    void UploadToGpu();
    void Destroy();

  private:
    SDL_GPUDevice *Gpu = nullptr;
};

}; // namespace gargantuan
