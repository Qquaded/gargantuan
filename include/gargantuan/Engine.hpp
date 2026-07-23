#pragma once

#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/render/MeshProvider.hpp"
#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <glm/gtc/matrix_transform.hpp>
#include <lua.h>
#include <memory>

namespace gargantuan {

class Engine {
  public:
    glm::vec2 ViewportSize = glm::vec2(720, 540);
    SDL_Window *Window = SDL_CreateWindow("Gargantuan", ViewportSize.x, ViewportSize.y, SDL_WINDOW_RESIZABLE);
    SDL_GPUDevice *Gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    MeshProvider MeshProvider = gargantuan::MeshProvider(Gpu);
    Renderer Renderer = gargantuan::Renderer(Window, Gpu, MeshProvider);
    ScriptEngine ScriptEngine;

    Engine();
    ~Engine();

    std::shared_ptr<DataModel> dataModel;
    bool IsRunning = true;
    void ProcessEvent(SDL_Event event);
    void Step();
    float GetDeltaTime() { return (CurrentTick - LastTick) / 1000.0f; };

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
