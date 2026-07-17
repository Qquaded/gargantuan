#include "gargantuan/Game.hpp"
#include "gargantuan/render/Renderer.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_timer.h>
#include <ext/vector_float3.hpp>
#include <geometric.hpp>

namespace gargantuan {

Game::Game() : Window(SDL_CreateWindow("Gargantuan", 720, 540, SDL_WINDOW_RESIZABLE)), Renderer(Window) {}

Game::~Game() { SDL_DestroyWindow(Window); }

void Game::ProcessEvent(SDL_Event event) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
        SDL_Log("Stopping");
        IsRunning = false;
        return;

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

    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
    }
        Renderer.OnWindowResize(event.window.data1, event.window.data2);
    }
}

void Game::Step() {
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

    if (keys[SDL_SCANCODE_W]) {
        CameraPosition += CameraLookVector * CameraSpeed;
    }

    if (keys[SDL_SCANCODE_S]) {
        CameraPosition -= CameraLookVector * CameraSpeed;
    }

    if (keys[SDL_SCANCODE_A]) {
        CameraPosition -= CameraRightVector * CameraSpeed;
    }

    if (keys[SDL_SCANCODE_D]) {
        CameraPosition += CameraRightVector * CameraSpeed;
    }

    if (keys[SDL_SCANCODE_SPACE]) {
        CameraPosition += glm::vec3(0, CameraSpeed, 0);
    }

    // Shift
    if (keys[SDL_SCANCODE_LSHIFT]) {
        CameraPosition -= glm::vec3(0, CameraSpeed, 0);
    }

    SDL_Log("FPS: %0.f", 1 / GetDeltaTime());
    Renderer.Draw(ModelProjectionView());

    LastTick = CurrentTick;
}

} // namespace gargantuan
