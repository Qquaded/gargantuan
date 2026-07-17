#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/render/Meshes.hpp"

#include <SDL3/SDL.h>

#include <SDL3/SDL_gpu.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ext/vector_float3.hpp>

namespace gargantuan::render {

Renderer::Renderer(SDL_Window *window) {
    this->Window = window;
    this->Gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

    if (!Gpu) {
        SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
        std::abort();
    }

    if (!SDL_ClaimWindowForGPUDevice(Gpu, window)) {
        SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
        std::abort();
    }

    SDL_Log("GPU driver: %s", SDL_GetGPUDeviceDriver(Gpu));

    VertexShader = LoadShader("build/shaders/rgbTriangle.vert.spv",
                              SDL_GPU_SHADERSTAGE_VERTEX);

    FragmentShader = LoadShader("build/shaders/rgbTriangle.frag.spv",
                                SDL_GPU_SHADERSTAGE_FRAGMENT);

    SDL_GPUColorTargetDescription colorTarget = {
        .format = SDL_GetGPUSwapchainTextureFormat(Gpu, Window)};

    SDL_GPUVertexAttribute vertexAttributes[]{
        SDL_GPUVertexAttribute{
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(Vertex, position),
        },
        SDL_GPUVertexAttribute{
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
            .offset = offsetof(Vertex, rgba),
        }};

    SDL_GPUVertexBufferDescription vertexBufferDescriptions[]{
        SDL_GPUVertexBufferDescription{.slot = 0,
                                       .pitch = sizeof(Vertex),
                                       .input_rate =
                                           SDL_GPU_VERTEXINPUTRATE_VERTEX}};

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {
        .vertex_shader = VertexShader,
        .fragment_shader = FragmentShader,
        .vertex_input_state =
            SDL_GPUVertexInputState{
                .vertex_buffer_descriptions = vertexBufferDescriptions,
                .num_vertex_buffers = 1,
                .vertex_attributes = vertexAttributes,
                .num_vertex_attributes = 2,
            },
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

    Pipeline = SDL_CreateGPUGraphicsPipeline(Gpu, &pipelineInfo);
    if (Pipeline == nullptr) {
        SDL_Log("SDL_CreateGPUGraphicsPipeline failed: %s", SDL_GetError());
        std::abort();
    }
}

Renderer::~Renderer() {
    SDL_WaitForGPUIdle(Gpu);
    SDL_ReleaseGPUGraphicsPipeline(Gpu, Pipeline);
    SDL_ReleaseGPUShader(Gpu, VertexShader);
    SDL_ReleaseGPUShader(Gpu, FragmentShader);
    SDL_ReleaseWindowFromGPUDevice(Gpu, Window);
    SDL_DestroyGPUDevice(Gpu);
}

SDL_GPUShader *Renderer::LoadShader(const char *filepath,
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

    SDL_GPUShader *shader = SDL_CreateGPUShader(Gpu, &createInfo);
    SDL_free(code);

    if (shader == nullptr) {
        SDL_Log("Failed to create shader file %s: %s", filepath,
                SDL_GetError());
        return nullptr;
    }

    return shader;
}

// TODO: proper pipeline/render passes
void Renderer::Draw() {
    Vertex vertices[]{
        {glm::vec3(0.0f, 0.5f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
    };

    uint32_t verticesSize = sizeof(vertices);

    SDL_GPUBufferCreateInfo bufferInfo{.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
                                       .size = verticesSize};
    auto vertexBuffer = SDL_CreateGPUBuffer(Gpu, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = verticesSize};
    auto transferBuffer = SDL_CreateGPUTransferBuffer(Gpu, &transferInfo);

    void *pointer = SDL_MapGPUTransferBuffer(Gpu, transferBuffer, false);
    memcpy(pointer, vertices, verticesSize);
    SDL_UnmapGPUTransferBuffer(Gpu, transferBuffer);

    {
        // Data upload pass
        auto commands = SDL_AcquireGPUCommandBuffer(Gpu);
        auto copyPass = SDL_BeginGPUCopyPass(commands);

        SDL_GPUTransferBufferLocation transferLocation{
            .transfer_buffer = transferBuffer, .offset = 0};

        SDL_GPUBufferRegion bufferRegion{
            .buffer = vertexBuffer, .offset = 0, .size = verticesSize};

        SDL_UploadToGPUBuffer(copyPass, &transferLocation, &bufferRegion,
                              false);

        SDL_EndGPUCopyPass(copyPass);

        // Render pass
        uint32_t width, height;
        SDL_GPUTexture *swapchainTexture;
        if (SDL_AcquireGPUSwapchainTexture(commands, Window, &swapchainTexture,
                                           &width, &height) &&
            swapchainTexture != nullptr) {

            SDL_GPUColorTargetInfo colorTarget = {
                .texture = swapchainTexture,
                .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
                .load_op = SDL_GPU_LOADOP_CLEAR,
                .store_op = SDL_GPU_STOREOP_STORE,
            };

            SDL_GPURenderPass *renderPass =
                SDL_BeginGPURenderPass(commands, &colorTarget, 1, nullptr);
            SDL_BindGPUGraphicsPipeline(renderPass, Pipeline);

            SDL_GPUBufferBinding binding{.buffer = vertexBuffer, .offset = 0};
            SDL_BindGPUVertexBuffers(renderPass, 0, &binding, 1);

            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(commands);
        SDL_WaitForGPUIdle(Gpu);
    }

    SDL_ReleaseGPUTransferBuffer(Gpu, transferBuffer);
    SDL_ReleaseGPUBuffer(Gpu, vertexBuffer);

    //     SDL_GPUTexture *swapchainTexture;
    //     uint32_t width, height;
    //     if (!SDL_AcquireGPUSwapchainTexture(commands, Window,
    //     &swapchainTexture,
    //                                         &width, &height)) {
    //         SDL_Log("SDL_AcquireGPUSwapchainTexture failed: %s",
    //         SDL_GetError()); SDL_CancelGPUCommandBuffer(commands); return;
    //     }

    //     if (swapchainTexture == nullptr) {
    //         SDL_CancelGPUCommandBuffer(commands);
    //         return;
    //     }
    // }

    // SDL_GPUColorTargetInfo colorTarget = {
    //     .texture = swapchainTexture,
    //     .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
    //     .load_op = SDL_GPU_LOADOP_CLEAR,
    //     .store_op = SDL_GPU_STOREOP_STORE,
    // };

    // SDL_GPURenderPass *renderPass =
    //     SDL_BeginGPURenderPass(commands, &colorTarget, 1, nullptr);

    // SDL_BindGPUGraphicsPipeline(renderPass, Pipeline);
    // SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

    // SDL_EndGPURenderPass(renderPass);

    // SDL_SubmitGPUCommandBuffer(commands);
}

} // namespace gargantuan::render
