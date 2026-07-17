#pragma once

#include "gargantuan/render/Meshes.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <cstdint>

namespace gargantuan::render {

class Renderer {
  public:
    // NOTE: Renderer does not take ownership of `window`
    Renderer(SDL_Window *window);
    ~Renderer();

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void Draw();
    SDL_GPUShader *LoadShader(const char *shaderPath, SDL_GPUShaderStage stage);

  private:
    SDL_Window *Window;
    SDL_GPUDevice *Gpu = nullptr;
    SDL_GPUGraphicsPipeline *Pipeline = nullptr;

    SDL_GPUShader *VertexShader = nullptr;
    SDL_GPUShader *FragmentShader = nullptr;

    StaticMesh *TestTriangle = nullptr;

    struct DrawContext {
        SDL_GPUCommandBuffer *commands;
        SDL_GPUTexture *targetTexture;
        uint32_t width;
        uint32_t height;
    };

    bool DrawTryStart(DrawContext &context);
    void DrawMainPass(DrawContext &context);
    void DrawEnd(DrawContext &context);
};

} // namespace gargantuan::render
