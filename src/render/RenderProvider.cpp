#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "gargantuan/render/RenderProvider.hpp"
#include "gargantuan/render/RenderPass.hpp"

#include <SDL3/SDL.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace gargantuan {

RenderProvider::RenderProvider(SDL_Window *window, SDL_GPUDevice *gpu) : Window(window), Gpu(gpu) {
    if (!SDL_ClaimWindowForGPUDevice(Gpu, Window)) {
        SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
        std::abort();
    }

    SwapchainFormat = SDL_GetGPUSwapchainTextureFormat(Gpu, Window);
    OpaquePass = CreateOpaquePass(Gpu, SwapchainFormat);

    int width, height;
    SDL_GetWindowSizeInPixels(Window, &width, &height);
    Resize(width, height);
}

void RenderProvider::Destroy() {
    SDL_WaitForGPUIdle(Gpu);
    if (DepthTexture != nullptr) {
        SDL_ReleaseGPUTexture(Gpu, DepthTexture);
    };
    SDL_ReleaseGPUGraphicsPipeline(Gpu, Pipeline);
    OpaquePass->Destroy(Gpu);
}

void RenderProvider::Draw(DrawContext drawContext) {
    SDL_GPUCommandBuffer *commands = SDL_AcquireGPUCommandBuffer(Gpu);
    if (!commands) {
        SDL_Log("Failed to acquire command buffer: %s", SDL_GetError());
        return;
    }

    FrameContext frameContext;
    frameContext.Commands = commands;
    frameContext.ViewMatrix = drawContext.ViewMatrix;
    frameContext.ProjectionMatrix = drawContext.ProjectionMatrix;
    frameContext.WorldRoot = drawContext.WorldRoot;

    if (DepthTexture) {
        frameContext.DepthTexture = DepthTexture;
    } else {
        SDL_CancelGPUCommandBuffer(frameContext.Commands);
        return;
    }

    auto swapchainResult = SDL_AcquireGPUSwapchainTexture(
        frameContext.Commands, Window, &frameContext.SwapchainTexture, &frameContext.Width, &frameContext.Height
    );
    if (!swapchainResult) {
        SDL_Log("Failed to acquire swapchain texture: %s", SDL_GetError());
        if (frameContext.Commands) {
            SDL_CancelGPUCommandBuffer(frameContext.Commands);
        };
        return;
    }

    SDL_GPUColorTargetInfo colorTarget = {
        .texture = frameContext.SwapchainTexture,
        .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    SDL_GPUDepthStencilTargetInfo depthTarget = {
        .texture = frameContext.DepthTexture,
        .clear_depth = 1.0f,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_DONT_CARE,
        .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
        .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
    };

    auto pass = SDL_BeginGPURenderPass(frameContext.Commands, &colorTarget, 1, &depthTarget);
    OpaquePass->Draw(Gpu, pass, frameContext);
    SDL_EndGPURenderPass(pass);

    SDL_SubmitGPUCommandBuffer(frameContext.Commands);
}

void RenderProvider::Resize(int width, int height) {
    if (width < 1 || height < 1) {
        return;
    }

    SDL_SetGPUSwapchainParameters(Gpu, Window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);

    if (DepthTexture != nullptr) {
        SDL_ReleaseGPUTexture(Gpu, DepthTexture);
    }

    SDL_GPUTextureCreateInfo depthInfo{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .width = (uint32_t)width,
        .height = (uint32_t)height,
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };

    DepthTexture = SDL_CreateGPUTexture(Gpu, &depthInfo);
}

} // namespace gargantuan
