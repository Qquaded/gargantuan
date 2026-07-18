#pragma once

#include "gargantuan/instances/list/DataModel.hpp"
#include "gargantuan/instances/list/Part.hpp"
#include "gargantuan/render/Renderer.hpp"

#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <lua.h>
#include <memory>

namespace gargantuan {

class Game {
  public:
    Game();
    ~Game();

    std::shared_ptr<instances::DataModel> dataModel;

    glm::vec2 ViewportSize = glm::vec2(720, 540);
    bool IsRunning = true;

    lua_State *Lua;

    void ProcessEvent(SDL_Event event);
    void Step();

    float GetDeltaTime() { return (CurrentTick - LastTick) / 1000.0f; };

  private:
    uint64_t CurrentTick;
    uint64_t LastTick;

    SDL_Window *Window;
    render::Renderer Renderer;

    // TODO: Camera class, and probably implement Instances by then
    glm::vec3 CameraPosition = glm::vec3(0, 0, 10);
    glm::vec3 CameraUpVector;
    glm::vec3 CameraRightVector;
    glm::vec3 CameraLookVector;
    float CameraYaw = -90.0f;
    float CameraPitch = 0.0f;
    float CameraSensitivity = 0.2f;
    float CameraSpeed = 10.0f;
    float CameraFieldOfView = 70.0f;

    glm::mat4 GetProjectionMatrix() {
        float aspect = ViewportSize.x / ViewportSize.y;
        return glm::perspective(glm::radians(CameraFieldOfView), aspect, 0.1f, 100.0f);
    }

    glm::mat4 GetViewMatrix() { return glm::lookAt(CameraPosition, CameraPosition + CameraLookVector, CameraUpVector); }

    std::shared_ptr<instances::Part> cube;
};

} // namespace gargantuan
