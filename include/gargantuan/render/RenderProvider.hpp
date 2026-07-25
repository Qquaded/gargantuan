#pragma once

#include "gargantuan/render/RenderPass.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <memory>

namespace gargantuan {

std::unique_ptr<RenderPass> CreateOpaquePass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat);

class RenderProvider {
  public:
    RenderProvider(SDL_Window *window, SDL_GPUDevice *gpu);

    RenderProvider(const RenderProvider &) = delete;
    RenderProvider &operator=(const RenderProvider &) = delete;

    void Draw(DrawContext drawContext);
    void Resize(int width, int height);
    void Destroy();

    SDL_Window *Window = nullptr;
    SDL_GPUDevice *Gpu = nullptr;
    SDL_GPUGraphicsPipeline *Pipeline = nullptr;
    SDL_GPUTexture *DepthTexture = nullptr;

    SDL_GPUTextureFormat SwapchainFormat;

    std::unique_ptr<RenderPass> OpaquePass;
};

} // namespace gargantuan
