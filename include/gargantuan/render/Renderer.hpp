#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

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
};

} // namespace gargantuan::render
