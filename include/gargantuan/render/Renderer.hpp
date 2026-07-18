#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <cstdint>

namespace gargantuan::render {

class Renderer {
  public:
    // NOTE: Renderer does not take ownership of `window`
    Renderer(SDL_Window *window);
    ~Renderer();

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    struct DrawInfo {
        std::shared_ptr<datatypes::Instance> worldModel;
        glm::mat4 projectionMatrix;
        glm::mat4 viewMatrix;
    };

    void Draw(DrawInfo info);
    void OnWindowResize(int width, int height);
    SDL_GPUShader *LoadShader(const char *shaderPath, SDL_GPUShaderStage stage);

    SDL_Window *Window = nullptr;
    SDL_GPUDevice *Gpu = nullptr;
    SDL_GPUGraphicsPipeline *Pipeline = nullptr;
    SDL_GPUTexture *DepthTexture = nullptr;

    SDL_GPUShader *VertexShader = nullptr;
    SDL_GPUShader *FragmentShader = nullptr;

    struct DrawContext {
        DrawInfo info;
        SDL_GPUCommandBuffer *commands;
        SDL_GPUTexture *targetTexture;
        uint32_t width;
        uint32_t height;
    };

    struct PushUniforms {
        glm::mat4 modelViewProjection;
    };

    bool DrawTryStart(DrawContext &context);
    void DrawMainPass(DrawContext &context);
    void DrawEnd(DrawContext &context);
};

} // namespace gargantuan::render
