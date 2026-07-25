#include "gargantuan/render/PipelineBuilder.hpp"
#include "gargantuan/render/RenderPass.hpp"
#include "gargantuan/render/RenderProvider.hpp"
#include "gargantuan/render/Shader.hpp"

#include <SDL3/SDL.h>
#include <memory>

namespace gargantuan {

class OpaquePass final : public RenderPass {
  public:
    struct CameraUniforms {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
    };

    struct PartUniforms {
        glm::mat4 ModelMatrix;
        glm::vec4 Rgba;
    };

    FileShader Shader{
        .VertexShaderFilepath = GetShaderPath("opaque.vert"),
        .VertexUniformBufferCount = 2,
        .FragmentShaderFilepath = GetShaderPath("opaque.frag"),
        .FragmentUniformBufferCount = 1
    };

    OpaquePass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
        Shader.Init(gpu);
        Pipeline = PipelineBuilder()
                       .SetVertexShader(Shader.VertexShader)
                       .SetFragmentShader(Shader.FragmentShader)
                       .SetColorFormat(swapchainFormat)
                       .SetBlendingEnabled(true)
                       .SetDepthEnabled(true)
                       .Build(gpu);
    };

    void Draw(SDL_GPUDevice *gpu, SDL_GPURenderPass *pass, const FrameContext &context) override {
        SDL_BindGPUGraphicsPipeline(pass, Pipeline);

        CameraUniforms cameraUniforms{
            .ViewMatrix = context.ViewMatrix,
            .ProjectionMatrix = context.ProjectionMatrix,
        };
        SDL_PushGPUVertexUniformData(context.Commands, 0, &cameraUniforms, sizeof(CameraUniforms));

        for (auto part : context.WorldRoot->Parts) {
            auto &mesh = part->GetMesh();
            if (!mesh || !mesh->VertexBuffer || !mesh->IndexBuffer) {
                continue;
            }

            CFrame cframe = part->CFrame;
            glm::mat4 model = cframe.Rotation;
            glm::vec3 position = cframe.Position;
            model[3] = glm::vec4(position, 1.0f);
            model = glm::scale(model, part->Size);

            PartUniforms uniforms{
                .ModelMatrix = model,
                .Rgba = glm::vec4((glm::vec3)part->Color, 1.0f - part->Transparency),
            };
            SDL_PushGPUVertexUniformData(context.Commands, 1, &uniforms, sizeof(PartUniforms));

            SDL_GPUBufferBinding vertexBinding{.buffer = mesh->VertexBuffer, .offset = 0};
            SDL_BindGPUVertexBuffers(pass, 0, &vertexBinding, 1);

            SDL_GPUBufferBinding indexBinding{.buffer = mesh->IndexBuffer, .offset = 0};
            SDL_BindGPUIndexBuffer(pass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

            SDL_DrawGPUIndexedPrimitives(pass, mesh->IndexCount, 1, 0, 0, 0);
        }
    };
};

std::unique_ptr<RenderPass> CreateOpaquePass(SDL_GPUDevice *gpu, SDL_GPUTextureFormat swapchainFormat) {
    return std::make_unique<OpaquePass>(gpu, swapchainFormat);
}

} // namespace gargantuan
