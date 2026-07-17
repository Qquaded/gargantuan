#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/render/Meshes.hpp"
#include "gargantuan/render/PrimitiveMeshes.hpp"

#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

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

    VertexShader = LoadShader("build/shaders/rgbTriangle.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);

    FragmentShader = LoadShader("build/shaders/rgbTriangle.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);

    SDL_GPUColorTargetDescription colorTarget = {.format = SDL_GetGPUSwapchainTextureFormat(Gpu, Window)};

    SDL_GPUVertexAttribute vertexAttributes[]{SDL_GPUVertexAttribute{
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

    SDL_GPUVertexBufferDescription vertexBufferDescriptions[]{SDL_GPUVertexBufferDescription{
        .slot = 0, .pitch = sizeof(Vertex), .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX}};

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
        .rasterizer_state = SDL_GPURasterizerState{.fill_mode = SDL_GPU_FILLMODE_FILL,
                                                   .cull_mode = SDL_GPU_CULLMODE_NONE,
                                                   .front_face = SDL_GPU_FRONTFACE_CLOCKWISE},
        .depth_stencil_state =
            SDL_GPUDepthStencilState{
                .compare_op = SDL_GPU_COMPAREOP_LESS,
                .back_stencil_state = {},
                .front_stencil_state = {},
                .compare_mask = 0,
                .write_mask = 0,
                .enable_depth_test = true,
                .enable_depth_write = true,
            },
        .target_info = SDL_GPUGraphicsPipelineTargetInfo{
            .color_target_descriptions = &colorTarget,
            .num_color_targets = 1,
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
            .has_depth_stencil_target = true,
        }};

    Pipeline = SDL_CreateGPUGraphicsPipeline(Gpu, &pipelineInfo);
    if (Pipeline == nullptr) {
        SDL_Log("SDL_CreateGPUGraphicsPipeline failed: %s", SDL_GetError());
        std::abort();
    }

    TestMesh = PrimitiveMeshes::Block(Gpu, glm::vec4(1.0, 1.0, 1.0, 1.0));

    int width, height;
    SDL_GetWindowSizeInPixels(Window, &width, &height);
    OnWindowResize(width, height);
}

Renderer::~Renderer() {
    SDL_WaitForGPUIdle(Gpu);
    if (DepthTexture != nullptr) {
        SDL_ReleaseGPUTexture(Gpu, DepthTexture);
    };
    SDL_ReleaseGPUGraphicsPipeline(Gpu, Pipeline);
    SDL_ReleaseGPUShader(Gpu, VertexShader);
    SDL_ReleaseGPUShader(Gpu, FragmentShader);
    SDL_ReleaseWindowFromGPUDevice(Gpu, Window);
    SDL_DestroyGPUDevice(Gpu);
}

SDL_GPUShader *Renderer::LoadShader(const char *filepath, SDL_GPUShaderStage stage) {
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
        .num_uniform_buffers = stage == SDL_GPU_SHADERSTAGE_VERTEX ? 1u : 0u,
    };

    SDL_GPUShader *shader = SDL_CreateGPUShader(Gpu, &createInfo);
    SDL_free(code);

    if (shader == nullptr) {
        SDL_Log("Failed to create shader file %s: %s", filepath, SDL_GetError());
        return nullptr;
    }

    return shader;
}

void Renderer::Draw(glm::mat4 modelViewProjection) {
    Renderer::DrawContext context;
    if (!DrawTryStart(context, modelViewProjection)) {
        return;
    }

    DrawMainPass(context);

    DrawEnd(context);
}

void Renderer::OnWindowResize(int width, int height) {
    if (width < 1 || height < 1) {
        return;
    }

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

bool Renderer::DrawTryStart(Renderer::DrawContext &context, glm::mat4 modelViewProjection) {
    if (DepthTexture == nullptr) {
        return false;
    }

    context.commands = SDL_AcquireGPUCommandBuffer(Gpu);
    if (context.commands == nullptr) {
        SDL_Log("SDL_AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return false;
    }

    if (!SDL_AcquireGPUSwapchainTexture(context.commands, Window, &context.targetTexture, &context.width,
                                        &context.height)) {
        SDL_Log("SDL_AcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        SDL_CancelGPUCommandBuffer(context.commands);
        return false;
    }

    if (context.targetTexture == nullptr) {
        SDL_CancelGPUCommandBuffer(context.commands);
        return false;
    }

    context.modelViewProjection = modelViewProjection;

    return true;
}

void Renderer::DrawMainPass(Renderer::DrawContext &context) {
    auto aspectRatio = (float)context.width / (float)context.height;

    Renderer::PushUniforms uniforms{.modelViewProjection = context.modelViewProjection};

    SDL_GPUColorTargetInfo colorTarget = {
        .texture = context.targetTexture,
        .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    SDL_GPUDepthStencilTargetInfo depthTarget = {
        .texture = DepthTexture,
        .clear_depth = 1.0f,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_DONT_CARE,
        .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
        .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
    };

    SDL_PushGPUVertexUniformData(context.commands, 0, &uniforms, sizeof(PushUniforms));

    auto renderPass = SDL_BeginGPURenderPass(context.commands, &colorTarget, 1, &depthTarget);
    {
        SDL_BindGPUGraphicsPipeline(renderPass, Pipeline);

        SDL_GPUBufferBinding binding{.buffer = TestMesh->VertexBuffer, .offset = 0};
        SDL_BindGPUVertexBuffers(renderPass, 0, &binding, 1);

        SDL_DrawGPUPrimitives(renderPass, TestMesh->VertexCount, 1, 0, 0);
    }
    SDL_EndGPURenderPass(renderPass);
}

void Renderer::DrawEnd(Renderer::DrawContext &context) { SDL_SubmitGPUCommandBuffer(context.commands); }

} // namespace gargantuan::render
