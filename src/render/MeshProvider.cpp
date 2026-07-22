#include "gargantuan/render/MeshProvider.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <memory>

namespace gargantuan {

MeshProvider::MeshProvider(SDL_GPUDevice *gpu) : Gpu(gpu) {}

void MeshProvider::Destroy() {
    for (auto &[meshId, gpuMesh] : GpuMeshes) {
        SDL_Log("deleting gpu mesh %s (%s)", meshId.data(), gpuMesh ? "exists" : "not exist");
        gpuMesh->Destroy(Gpu);
    }
    GpuMeshes.clear();
}

void MeshProvider::UploadToGpu() {
    if (UnloadedMeshes.empty()) {
        return;
    }

    auto cmd = SDL_AcquireGPUCommandBuffer(Gpu);
    auto copyPass = SDL_BeginGPUCopyPass(cmd);

    for (auto &[meshId, unloadedMesh] : UnloadedMeshes) {
        // if (auto &gpuMesh = GpuMeshes.find(meshId)) {
        //     gpuMesh->Destroy(Gpu);
        // };

        auto gpuMesh = std::make_unique<GpuMesh>(unloadedMesh);
        gpuMesh->Upload(Gpu, copyPass);
        GpuMeshes[meshId] = std::move(gpuMesh);
    }

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    UnloadedMeshes.clear();
}

} // namespace gargantuan
