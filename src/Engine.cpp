#include "gargantuan/Engine.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/instances/list/Part.hpp"
#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/scripting/Runtime.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <cstdlib>
#include <cstring>
#include <glm/glm.hpp>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>

namespace gargantuan {

Engine::Engine()
    : Window(SDL_CreateWindow("Gargantuan", ViewportSize.x, ViewportSize.y, SDL_WINDOW_RESIZABLE)), Renderer(Window),
      ScriptEngine() {
    dataModel = std::make_shared<instances::DataModel>();
    dataModel->Name = "Welcome To Hell";

    // temporary
    Types::INSTANCE.PushStackValue(ScriptEngine.L, dataModel.get());
    lua_pushvalue(ScriptEngine.L, -1);
    lua_setglobal(ScriptEngine.L, "game");

    auto baseplate = std::make_shared<instances::Part>();
    baseplate->Name = "Baseplate";
    baseplate->Color = datatypes::Color3::fromHSV(0, 0, 0.5);
    baseplate->CFrame = datatypes::CFrame(0, -30, 0);
    baseplate->Size = glm::vec3(50, 20, 50);
    baseplate->Transparency = 1.0;
    baseplate->SetParent(dataModel);

    cube = std::make_shared<instances::Part>();
    cube->Name = "Cube";
    cube->Color = datatypes::Color3::fromHSV(1, 1, 1);
    cube->CFrame = datatypes::CFrame(0, 5, 0);
    cube->Transparency = 1.0;
    cube->SetParent(dataModel);

    SDL_Log("DataModel has %d descendants", (int)dataModel->GetDescendants().size());
}

Engine::~Engine() {
    if (dataModel) {
        dataModel->Children.clear();
        dataModel.reset();
    }
    SDL_DestroyWindow(Window);
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

    constexpr float CYCLE_DURATION = 5;
    float timeSec = (float)CurrentTick / 1000.0f;

    auto cubeHue = glm::mod((float)CurrentTick / 1000.0f, CYCLE_DURATION) / CYCLE_DURATION;
    glm::vec3 cubePosition = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 lookTarget = cubePosition + glm::vec3(glm::cos(timeSec), glm::cos(timeSec), glm::sin(timeSec));
    cube->CFrame = datatypes::CFrame(cubePosition, lookTarget);
    cube->Color = datatypes::Color3::fromHSV(cubeHue, 1, 1);
    cube->UploadGeometry(Renderer.Gpu);

    Renderer.Draw(
        render::Renderer::DrawInfo{
            .worldModel = dataModel,
            .projectionMatrix = GetProjectionMatrix(),
            .viewMatrix = GetViewMatrix(),
        }
    );

    LastTick = CurrentTick;

    ScriptEngine.Step();
}

} // namespace gargantuan
