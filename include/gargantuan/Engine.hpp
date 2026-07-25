#pragma once

#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/render/RenderProvider.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/services/RunService.hpp"
#include "gargantuan/services/Workspace.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <lua.h>
#include <memory>

namespace gargantuan {

class Engine {
  public:
    bool IsRunning = true;
    glm::vec2 ViewportSize = glm::vec2(720, 540);
    std::shared_ptr<DataModel> DataModel = nullptr;
    std::shared_ptr<Workspace> Workspace = nullptr;
    std::shared_ptr<RunService> RunService = nullptr;

    SDL_Window *Window;
    SDL_GPUDevice *Gpu;
    RenderProvider *RenderProvider;
    ScriptEngine *ScriptEngine;

    Engine();
    ~Engine();

    float GetDeltaTime() { return (CurrentTick - LastTick) / 1000.0f; };
    void ProcessEvent(SDL_Event event);
    void Step();

  private:
    uint64_t CurrentTick;
    uint64_t LastTick;

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
        return glm::perspective(glm::radians(CameraFieldOfView), aspect, 0.1f, 100000.0f);
    }

    glm::mat4 GetViewMatrix() { return glm::lookAt(CameraPosition, CameraPosition + CameraLookVector, CameraUpVector); }
};

} // namespace gargantuan
