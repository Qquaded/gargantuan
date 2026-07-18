#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <glm.hpp>

namespace gargantuan::render {

struct Vertex {
  public:
    glm::vec3 Position;
    glm::vec4 Rgba;
};

struct Mesh {
  public:
    const Vertex *Vertices;
    uint32_t VertexCount;
};

class GpuMesh : public Mesh {
  public:
    SDL_GPUBuffer *VertexBuffer = nullptr;

    GpuMesh(SDL_GPUDevice *gpu, const Vertex *vertices, uint32_t vertexCount);
    GpuMesh(SDL_GPUDevice *gpu, Mesh mesh);
    ~GpuMesh();

  private:
    SDL_GPUDevice *Gpu;
};

} // namespace gargantuan::render
