#pragma once

#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/MeshProvider.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <cstdint>

namespace gargantuan {

class Renderer {
  public:
    Renderer(SDL_Window *window, SDL_GPUDevice *gpu, MeshProvider &meshProvider);
    ~Renderer();

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    struct DrawInfo {
        Instance::Pointer worldModel;
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
    };

    void Draw(DrawInfo info);
    void OnWindowResize(int width, int height);
    SDL_GPUShader *LoadShader(const char *shaderPath, SDL_GPUShaderStage stage);

    MeshProvider &MeshProvider;

    SDL_Window *Window = nullptr;
    SDL_GPUDevice *Gpu = nullptr;
    SDL_GPUGraphicsPipeline *Pipeline = nullptr;
    SDL_GPUTexture *DepthTexture = nullptr;

    SDL_GPUShader *VertexShader = nullptr;
    SDL_GPUShader *FragmentShader = nullptr;

    struct DrawContext {
        DrawInfo info;
        SDL_GPUCommandBuffer *commands;
        SDL_GPUTexture *targetTexture;
        uint32_t width;
        uint32_t height;
    };

    struct PushUniforms {
        glm::mat4 mvp;
        glm::vec4 rgba;
    };

    bool DrawTryStart(DrawContext &context);
    void DrawMainPass(DrawContext &context);
    void DrawEnd(DrawContext &context);
};

} // namespace gargantuan
