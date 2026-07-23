#include "gargantuan/Engine.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/services/Workspace.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cstdlib>
#include <cstring>
#include <fwd.hpp>
#include <glm/glm.hpp>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <memory>
#include <stdexcept>

namespace gargantuan {

Engine::Engine() {
    this->Gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!Gpu) {
        throw std::runtime_error("Failed to instantiate GPU");
    }

    this->Window =
        SDL_CreateWindow("Gargantuan", ViewportSize.x, ViewportSize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    if (!Window) {
        throw std::runtime_error("Failed to instantiate window");
    }

    this->MeshProvider = new class MeshProvider(Gpu);
    this->Renderer = new class Renderer(Window, Gpu, *MeshProvider);
    this->ScriptEngine = new class ScriptEngine();

    DataModel = std::make_shared<gargantuan::DataModel>();
    DataModel->Name = "Welcome To Hell";

    auto workspace = this->DataModel->GetService("Workspace");
    this->Workspace = std::dynamic_pointer_cast<gargantuan::Workspace>(workspace);

    auto runService = this->DataModel->GetService("RunService");
    this->RunService = std::dynamic_pointer_cast<gargantuan::RunService>(runService);

    SDL_Log("children (c++) has %zu", workspace->GetChildren().size());

    StackValue<Instance::Pointer>::Push(ScriptEngine->L, this->DataModel);
    lua_pushvalue(ScriptEngine->L, -1);
    lua_setglobal(ScriptEngine->L, "game");
}

Engine::~Engine() {
    SDL_Log("destroying window");
    SDL_ReleaseWindowFromGPUDevice(Gpu, Window);
    SDL_DestroyWindow(Window);

    SDL_Log("destroying mesh provider");
    MeshProvider->Destroy();

    Renderer->Destroy();

    SDL_Log("destroying gpu %s", Gpu ? "exists" : "not exist");
    SDL_DestroyGPUDevice(Gpu);
    Gpu = nullptr;
    SDL_Log("done destroying gpu");
}

void Engine::ProcessEvent(SDL_Event event) {
    switch (event.type) {

    case SDL_EVENT_QUIT:
        SDL_Log("Stopping");
        IsRunning = false;
        return;

    case SDL_EVENT_WINDOW_RESIZED:
        ViewportSize.x = event.window.data1;
        ViewportSize.y = event.window.data2;
        SDL_Log("Resizing: %0.fx%0.f", ViewportSize.x, ViewportSize.y);
        Renderer->OnWindowResize(ViewportSize.x, ViewportSize.y);
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_RIGHT) {
            SDL_SetWindowRelativeMouseMode(Window, true);
        };
        return;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_RIGHT) {
            SDL_SetWindowRelativeMouseMode(Window, false);
        };
        return;

    case SDL_EVENT_MOUSE_MOTION:
        if (SDL_GetWindowRelativeMouseMode(Window)) {
            CameraYaw += event.motion.xrel * CameraSensitivity;
            CameraPitch -= event.motion.yrel * CameraSensitivity;

            if (CameraPitch > 89.0f) {
                CameraPitch = 89.0f;
            };

            if (CameraPitch < -89.0f) {
                CameraPitch = -89.0f;
            };

            float yawRad = glm::radians(CameraYaw);
            float pitchRad = glm::radians(CameraPitch);
            auto worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

            CameraLookVector =
                glm::normalize(glm::vec3(cos(yawRad) * cos(pitchRad), sin(pitchRad), sin(yawRad) * cos(pitchRad)));

            CameraRightVector = glm::normalize(glm::cross(CameraLookVector, worldUp));
            CameraUpVector = glm::normalize(glm::cross(CameraRightVector, CameraLookVector));
        };
        return;
    }
}

void Engine::Step() {
    CurrentTick = SDL_GetTicks();
    if (!LastTick) {
        LastTick = SDL_GetTicks();
    };

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ProcessEvent(event);
    }

    if (!IsRunning) {
        return;
    }

    int numKeys;
    auto keys = SDL_GetKeyboardState(&numKeys);
    auto worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    auto deltaTime = GetDeltaTime();

    if (keys[SDL_SCANCODE_W]) {
        CameraPosition += CameraLookVector * CameraSpeed * deltaTime;
    }

    if (keys[SDL_SCANCODE_S]) {
        CameraPosition -= CameraLookVector * CameraSpeed * deltaTime;
    }

    if (keys[SDL_SCANCODE_A]) {
        CameraPosition -= CameraRightVector * CameraSpeed * deltaTime;
    }

    if (keys[SDL_SCANCODE_D]) {
        CameraPosition += CameraRightVector * CameraSpeed * deltaTime;
    }

    if (keys[SDL_SCANCODE_SPACE]) {
        CameraPosition += glm::vec3(0, CameraSpeed * deltaTime, 0);
    }

    if (keys[SDL_SCANCODE_LSHIFT]) {
        CameraPosition -= glm::vec3(0, CameraSpeed * deltaTime, 0);
    }

    MeshProvider->UploadToGpu();
    Renderer->Draw(
        Renderer::DrawInfo{
            .WorldRoot = std::static_pointer_cast<WorldRoot>(Workspace),
            .ProjectionMatrix = GetProjectionMatrix(),
            .ViewMatrix = GetViewMatrix(),
        }
    );

    LastTick = CurrentTick;
    ScriptEngine->Step();
    RunService->PreRender->Fire(GetDeltaTime());
}

} // namespace gargantuan
