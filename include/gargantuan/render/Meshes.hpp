#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <glm.hpp>
#include <vector>

namespace gargantuan {

struct Vertex {
  public:
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec4 Rgba;
    glm::vec2 UV;
};

struct Mesh {
  public:
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
};

class GpuMesh : public Mesh {
  public:
    SDL_GPUBuffer *VertexBuffer = nullptr;
    SDL_GPUBuffer *IndexBuffer = nullptr;
    uint32_t VertexCount;
    uint32_t IndexCount;

    GpuMesh(SDL_GPUDevice *gpu, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    GpuMesh(SDL_GPUDevice *gpu, Mesh mesh);
    ~GpuMesh();

  private:
    SDL_GPUDevice *Gpu;
};

} // namespace gargantuan
