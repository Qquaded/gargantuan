#include "renderer/Renderer.h"
#include <chrono>
#include <thread>

int main(int argc, char *argv[]) {
  Renderer renderer;

  renderer.init();

  SDL_Event event;
  while (renderer.isRunning) {
    while (SDL_PollEvent(&event)) {
      renderer.handleEvent(event);
    }

    if (renderer.isRendering) {
      renderer.render();
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  renderer.destroy();
  return 0;
}
