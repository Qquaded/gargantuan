#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

namespace Gargantuan {

class Renderer {
  public:
    Renderer(SDL_Window *window);
    ~Renderer();

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void Draw();
    SDL_GPUShader *LoadShader(const char *shaderPath, SDL_GPUShaderStage stage);

  private:
    SDL_Window *window;
    SDL_GPUDevice *gpu = nullptr;
    SDL_GPUGraphicsPipeline *pipeline = nullptr;

    SDL_GPUShader *vertexShader = nullptr;
    SDL_GPUShader *fragmentShader = nullptr;
};

} // namespace Gargantuan
