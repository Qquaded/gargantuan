#pragma once

#include "gargantuan/render/Renderer.hpp"

#include <SDL3/SDL_video.h>

namespace gargantuan {

class Game {
  public:
    Game();
    ~Game();

    bool IsRunning = true;

    void ProcessEvent(SDL_Event event);
    void Step();

  private:
    SDL_Window *Window;
    render::Renderer Renderer;
};

} // namespace gargantuan
