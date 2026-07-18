#pragma once

#include "gargantuan/render/Renderer.hpp"

#include <SDL3/SDL_video.h>
#include <ext/vector_float2.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gargantuan {

class Game {
  public:
    Game();
    ~Game();

    glm::vec2 ViewportSize = glm::vec2(720, 540);
    bool IsRunning = true;

    void ProcessEvent(SDL_Event event);
    void Step();

    float GetDeltaTime() { return (CurrentTick - LastTick) / 1000.0f; };

  private:
    uint64_t CurrentTick;
    uint64_t LastTick;

    SDL_Window *Window;
    render::Renderer Renderer;

    // TODO: Camera class, and probably implement Instances by then
    glm::vec3 CameraPosition = glm::vec3(0, 0, 5);
    glm::vec3 CameraUpVector;
    glm::vec3 CameraRightVector;
    glm::vec3 CameraLookVector;
    float CameraYaw = -90.0f;
    float CameraPitch = 0.0f;
    float CameraSensitivity = 0.2f;
    float CameraSpeed = 0.1f;
    float CameraFieldOfView = 45.0f;

    glm::mat4 ModelProjectionView() {
        float aspect = ViewportSize.x / ViewportSize.y;

        auto model = glm::mat4(1.0f);
        auto projection = glm::perspective(glm::radians(CameraFieldOfView), aspect, 0.1f, 100.0f);
        auto view = glm::lookAt(CameraPosition, CameraPosition + CameraLookVector, CameraUpVector);

        return projection * view * model;
    }
};

} // namespace gargantuan
