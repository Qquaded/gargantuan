#pragma once

#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/render/Shader.hpp"

#include <SDL3/SDL.h>
#include <memory>

namespace gargantuan {

struct DrawContext {
    std::shared_ptr<WorldRoot> WorldRoot;
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
};

struct FrameContext : DrawContext {
    SDL_GPUCommandBuffer *Commands;
    SDL_GPUTexture *SwapchainTexture;
    SDL_GPUTexture *DepthTexture;
    uint32_t Width;
    uint32_t Height;
};

class RenderPass {
  public:
    Shader Shader;
    SDL_GPUGraphicsPipeline *Pipeline = nullptr;

    virtual ~RenderPass() = default;
    virtual void Draw(SDL_GPUDevice *gpu, SDL_GPURenderPass *pass, const FrameContext &context) = 0;
    virtual void Resize(SDL_GPUDevice *gpu, uint32_t width, uint32_t height) {};
    virtual void Destroy(SDL_GPUDevice *gpu);
};

} // namespace gargantuan
