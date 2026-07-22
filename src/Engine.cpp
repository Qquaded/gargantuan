#include "gargantuan/Engine.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/render/Renderer.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <cstdlib>
#include <cstring>
#include <fwd.hpp>
#include <glm/glm.hpp>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>

namespace gargantuan {

Engine::Engine() {
    dataModel = std::make_shared<DataModel>();
    dataModel->Name = "Welcome To Hell";
    StackValue<Instance::Pointer>::Push(ScriptEngine.L, dataModel);
    lua_pushvalue(ScriptEngine.L, -1);
    lua_setglobal(ScriptEngine.L, "game");
}

Engine::~Engine() {
    SDL_Log("destroying window");
    SDL_ReleaseWindowFromGPUDevice(Gpu, Window);
    SDL_DestroyWindow(Window);

    SDL_Log("destroying mesh provider");
    MeshProvider.Destroy();

    Renderer.Destroy();

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
        Renderer.OnWindowResize(ViewportSize.x, ViewportSize.y);
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

    MeshProvider.UploadToGpu();
    Renderer.Draw(
        Renderer::DrawInfo{
            .worldModel = dataModel,
            .projectionMatrix = GetProjectionMatrix(),
            .viewMatrix = GetViewMatrix(),
        }
    );

    LastTick = CurrentTick;

    ScriptEngine.Step();
}

} // namespace gargantuan
