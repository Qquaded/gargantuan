#include "gargantuan/render/Meshes.hpp"

#include <SDL3/SDL_gpu.h>

namespace gargantuan::render {

Mesh::Mesh(SDL_GPUDevice *gpu, const Vertex *vertices, uint32_t vertexCount) : Gpu(gpu), VertexCount(vertexCount) {
    uint32_t bufferSize = vertexCount * sizeof(Vertex);
    SDL_GPUBufferCreateInfo bufferInfo{.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = bufferSize};
    VertexBuffer = SDL_CreateGPUBuffer(Gpu, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = bufferSize};
    auto transferBuffer = SDL_CreateGPUTransferBuffer(Gpu, &transferInfo);

    void *pointer = SDL_MapGPUTransferBuffer(Gpu, transferBuffer, false);
    std::memcpy(pointer, vertices, bufferSize);
    SDL_UnmapGPUTransferBuffer(Gpu, transferBuffer);

    auto commands = SDL_AcquireGPUCommandBuffer(Gpu);
    auto copyPass = SDL_BeginGPUCopyPass(commands);
    {
        SDL_GPUTransferBufferLocation transferLocation{.transfer_buffer = transferBuffer, .offset = 0};
        SDL_GPUBufferRegion bufferRegion{.buffer = VertexBuffer, .offset = 0, .size = bufferSize};
        SDL_UploadToGPUBuffer(copyPass, &transferLocation, &bufferRegion, false);
    }
    SDL_EndGPUCopyPass(copyPass);

    SDL_SubmitGPUCommandBuffer(commands);
    SDL_ReleaseGPUTransferBuffer(Gpu, transferBuffer);
}

Mesh::~Mesh() {
    if (VertexBuffer) {
        SDL_ReleaseGPUBuffer(Gpu, VertexBuffer);
    }
}

} // namespace gargantuan::render
