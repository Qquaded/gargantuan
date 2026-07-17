#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <glm.hpp>

namespace gargantuan::render {

struct Vertex {
    glm::vec3 position;
    glm::vec4 rgba;
};

class StaticMesh {
  public:
    StaticMesh(SDL_GPUDevice *gpu, const Vertex *vertices, uint32_t vertexCount);
    ~StaticMesh();

    StaticMesh(const StaticMesh &) = delete;
    StaticMesh &operator=(const StaticMesh &) = delete;

    SDL_GPUBuffer *VertexBuffer = nullptr;
    uint32_t VertexCount = 0;

  private:
    SDL_GPUDevice *Gpu;
};

} // namespace gargantuan::render
