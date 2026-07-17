#include "gargantuan/Renderer.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdlib>

Gargantuan::Renderer::Renderer(SDL_Window *window) {
    this->window = window;
    this->gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

    if (!gpu) {
        SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
        std::abort();
    }

    if (!SDL_ClaimWindowForGPUDevice(gpu, window)) {
        SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
        std::abort();
    }

    SDL_Log("GPU driver: %s", SDL_GetGPUDeviceDriver(gpu));

    vertexShader = LoadShader("build/shaders/rgbTriangle.vert.spv",
                              SDL_GPU_SHADERSTAGE_VERTEX);

    fragmentShader = LoadShader("build/shaders/rgbTriangle.frag.spv",
                                SDL_GPU_SHADERSTAGE_FRAGMENT);

    SDL_GPUColorTargetDescription colorTarget = {
        .format = SDL_GetGPUSwapchainTextureFormat(gpu, window)};

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state =
            SDL_GPURasterizerState{
                .fill_mode = SDL_GPU_FILLMODE_FILL,
            },
        .target_info =
            SDL_GPUGraphicsPipelineTargetInfo{
                .color_target_descriptions = &colorTarget,
                .num_color_targets = 1,
            },
    };

    pipeline = SDL_CreateGPUGraphicsPipeline(gpu, &pipelineInfo);
    if (pipeline == nullptr) {
        SDL_Log("SDL_CreateGPUGraphicsPipeline failed: %s", SDL_GetError());
        std::abort();
    }
}

Gargantuan::Renderer::~Renderer() {
    SDL_WaitForGPUIdle(gpu);
    SDL_ReleaseGPUGraphicsPipeline(gpu, pipeline);
    SDL_ReleaseGPUShader(gpu, vertexShader);
    SDL_ReleaseGPUShader(gpu, fragmentShader);
    SDL_ReleaseWindowFromGPUDevice(gpu, window);
    SDL_DestroyGPUDevice(gpu);
}

SDL_GPUShader *Gargantuan::Renderer::LoadShader(const char *filepath,
                                                SDL_GPUShaderStage stage) {
    size_t codeSize;
    void *code = SDL_LoadFile(filepath, &codeSize);
    if (code == nullptr) {
        SDL_Log("Failed to open shader file %s", filepath);
        return nullptr;
    }

    SDL_GPUShaderCreateInfo createInfo = {
        .code_size = codeSize,
        .code = (const Uint8 *)code,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = stage,
        .num_samplers = 0,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 0,
    };

    SDL_GPUShader *shader = SDL_CreateGPUShader(gpu, &createInfo);
    SDL_free(code);

    if (shader == nullptr) {
        SDL_Log("Failed to create shader file %s: %s", filepath,
                SDL_GetError());
        return nullptr;
    }

    return shader;
}

void Gargantuan::Renderer::Draw() {
    auto commands = SDL_AcquireGPUCommandBuffer(gpu);
    if (!commands) {
        SDL_Log("SDL_AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        std::abort();
    }

    SDL_GPUTexture *swapchainTexture;
    uint32_t width, height;
    if (!SDL_AcquireGPUSwapchainTexture(commands, window, &swapchainTexture,
                                        &width, &height)) {
        SDL_Log("SDL_AcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commands);
        return;
    }

    SDL_GPUColorTargetInfo colorTarget = {
        .texture = swapchainTexture,
        .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    SDL_GPURenderPass *renderPass =
        SDL_BeginGPURenderPass(commands, &colorTarget, 1, nullptr);

    SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

    SDL_EndGPURenderPass(renderPass);

    SDL_SubmitGPUCommandBuffer(commands);
}
