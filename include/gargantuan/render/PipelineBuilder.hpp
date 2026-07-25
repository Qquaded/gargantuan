#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <SDL3/SDL.h>
#include <string_view>

namespace gargantuan {

struct PipelineBuilder {
  public:
    SDL_GPUShader *VertexShader = nullptr;
    SDL_GPUShader *FragmentShader = nullptr;

    SDL_GPUTextureFormat ColorFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    bool ColorEnabled = false;
    bool BlendingEnabled = false;

    SDL_GPUTextureFormat DepthFormat = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
    bool DepthEnabled = false;

    PipelineBuilder &SetVertexShader(SDL_GPUShader *shader);
    PipelineBuilder &SetFragmentShader(SDL_GPUShader *shader);
    PipelineBuilder &SetShaderFromCache(SDL_GPUDevice *gpu, std::string_view alias);
    PipelineBuilder &SetColorFormat(SDL_GPUTextureFormat format);
    PipelineBuilder &SetColorEnabled(bool enabled);
    PipelineBuilder &SetBlendingEnabled(bool enabled);
    PipelineBuilder &SetDepthFormat(SDL_GPUTextureFormat format);
    PipelineBuilder &SetDepthEnabled(bool enabled);

    SDL_GPUGraphicsPipeline *Build(SDL_GPUDevice *gpu);
};

} // namespace gargantuan
