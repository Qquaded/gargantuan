#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "gargantuan/render/PipelineBuilder.hpp"
#include "gargantuan/render/RenderPass.hpp"
#include "gargantuan/render/RenderProvider.hpp"
#include "gargantuan/render/Shader.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <memory>

namespace gargantuan {

class ShadowPass final : public RenderPass {
  public:
    struct LightUniforms {
        glm::mat4 LightSpaceMatrix;
    };

    struct PartUniforms {
        glm::mat4 ModelMatrix;
    };

    FileShader Shader{
        .VertexFilepath = GetShaderPath("shadow.vert"),
        .VertexUniformBufferCount = 2,
        .FragmentFilepath = GetShaderPath("shadow.frag"),
        .FragmentUniformBufferCount = 0,
    };

    ShadowPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
        Shader.Init(gpu);

        Pipeline = PipelineBuilder()
                       .SetVertexShader(Shader.VertexShader)
                       .SetFragmentShader(Shader.FragmentShader)
                       .SetColorFormat(swapchainFormat)
                       .SetColorEnabled(false)
                       .SetDepthEnabled(true)
                       .SetDepthFormat(SDL_GPU_TEXTUREFORMAT_D32_FLOAT)
                       .Build(gpu);
    };

    SDL_GPURenderPass *Draw(SDL_GPUDevice *gpu, FrameContext &context) override {
        glm::vec3 lightDirection = glm::normalize(context.LightDirection);
        glm::vec3 lightPosition = lightDirection * 20.0f;
        glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
        context.LightSpaceMatrix = lightProjection * lightView;

        SDL_GPUDepthStencilTargetInfo depthTarget{
            .texture = context.ShadowMapTexture,
            .clear_depth = 1.0f,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
            .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
        };

        SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(context.Commands, nullptr, 0, &depthTarget);
        SDL_BindGPUGraphicsPipeline(pass, Pipeline);

        LightUniforms lightUniforms{.LightSpaceMatrix = context.LightSpaceMatrix};
        SDL_PushGPUVertexUniformData(context.Commands, 0, &lightUniforms, sizeof(LightUniforms));

        for (auto part : context.WorldRoot->Parts) {
            auto &mesh = part->GetMesh();
            if (!mesh || !mesh->VertexBuffer || !mesh->IndexBuffer) {
                continue;
            }

            PartUniforms uniforms{.ModelMatrix = part->GetModelMatrix()};
            SDL_PushGPUVertexUniformData(context.Commands, 1, &uniforms, sizeof(PartUniforms));

            SDL_GPUBufferBinding vertexBinding{.buffer = mesh->VertexBuffer, .offset = 0};
            SDL_BindGPUVertexBuffers(pass, 0, &vertexBinding, 1);

            SDL_GPUBufferBinding indexBinding{.buffer = mesh->IndexBuffer, .offset = 0};
            SDL_BindGPUIndexBuffer(pass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

            SDL_DrawGPUIndexedPrimitives(pass, mesh->IndexCount, 1, 0, 0, 0);
        }

        return pass;
    };
};

std::unique_ptr<RenderPass> CreateShadowPass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
    return std::make_unique<ShadowPass>(gpu, swapchainFormat);
}

} // namespace gargantuan
