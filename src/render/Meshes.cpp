#include "gargantuan/render/Meshes.hpp"

#include <SDL3/SDL.h>
#include <cstdint>

namespace gargantuan::render {

GpuMesh::GpuMesh(SDL_GPUDevice *gpu, std::vector<Vertex> vertices, std::vector<uint32_t> indices) : Gpu(gpu) {
    Vertices = vertices;
    VertexCount = Vertices.size();
    Indices = indices;
    IndexCount = Indices.size();

    uint32_t vertexBufferSize = VertexCount * sizeof(Vertex);
    uint32_t indexBufferSize = IndexCount * sizeof(uint32_t);

    SDL_GPUBufferCreateInfo vertexBufferInfo{.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = vertexBufferSize};
    VertexBuffer = SDL_CreateGPUBuffer(Gpu, &vertexBufferInfo);

    SDL_GPUBufferCreateInfo indexBufferInfo{.usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = indexBufferSize};
    IndexBuffer = SDL_CreateGPUBuffer(Gpu, &indexBufferInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo{.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
                                                 .size = vertexBufferSize + indexBufferSize};
    auto transferBuffer = SDL_CreateGPUTransferBuffer(Gpu, &transferInfo);

    void *pointer = SDL_MapGPUTransferBuffer(Gpu, transferBuffer, false);
    std::memcpy(pointer, Vertices.data(), vertexBufferSize);
    std::memcpy((uint8_t *)pointer + vertexBufferSize, Indices.data(), indexBufferSize);
    SDL_UnmapGPUTransferBuffer(Gpu, transferBuffer);

    auto commands = SDL_AcquireGPUCommandBuffer(Gpu);
    auto copyPass = SDL_BeginGPUCopyPass(commands);
    {
        SDL_GPUTransferBufferLocation vertexSource{.transfer_buffer = transferBuffer, .offset = 0};
        SDL_GPUBufferRegion vertexDestination{.buffer = VertexBuffer, .offset = 0, .size = vertexBufferSize};
        SDL_UploadToGPUBuffer(copyPass, &vertexSource, &vertexDestination, false);

        SDL_GPUTransferBufferLocation indexSource{.transfer_buffer = transferBuffer, .offset = vertexBufferSize};
        SDL_GPUBufferRegion indexDestination{.buffer = IndexBuffer, .offset = 0, .size = indexBufferSize};
        SDL_UploadToGPUBuffer(copyPass, &indexSource, &indexDestination, false);
    }
    SDL_EndGPUCopyPass(copyPass);

    SDL_SubmitGPUCommandBuffer(commands);
    SDL_ReleaseGPUTransferBuffer(Gpu, transferBuffer);
}

GpuMesh::GpuMesh(SDL_GPUDevice *gpu, Mesh mesh) : GpuMesh(gpu, mesh.Vertices, mesh.Indices) {}

GpuMesh::~GpuMesh() {
    if (VertexBuffer) {
        SDL_ReleaseGPUBuffer(Gpu, VertexBuffer);
    }

    if (IndexBuffer) {
        SDL_ReleaseGPUBuffer(Gpu, IndexBuffer);
    }
}

} // namespace gargantuan::render
