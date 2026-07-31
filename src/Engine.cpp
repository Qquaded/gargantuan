#include "gargantuan/Engine.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/Profiler.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/render/Renderer.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <glm/glm.hpp>
#include <lua.h>
#include <memory>

namespace gargantuan {
	Engine::Engine(std::shared_ptr<gargantuan::DataModel> game, BaseRenderer *renderer)
		: DataModel(game), Renderer(renderer), Script(new class ScriptEngine(game)) {
		G_LOG_INFO("Constructed engine");
	}

	void Engine::Destroy() {
		G_LOG_INFO("Destroying engine");
		Renderer->Destroy();
	}

	float Engine::GetDeltaTime() {
		return (CurrentTick - LastTick) / 1000.0f;
	}

	void Engine::Step() {
		if (!IsRunning) return;

		CurrentTick = SDL_GetTicks();
		if (LastTick == 0) LastTick = CurrentTick;
		float deltaTime = GetDeltaTime();

		{
			G_PROFILE("Main Thread");

			{
				G_PROFILE("Events");

				SDL_Event event;
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_EVENT_WINDOW_RESIZED:
						Renderer->Resize(event.window.data1, event.window.data2);
						break;

					case SDL_EVENT_QUIT:
						G_LOG_INFO("Stopping engine");
						IsRunning = false;
						return;
					}
					UserInputService->ProcessEvent(event);
					Workspace->CurrentCamera->OnEvent(event);
				}
			}

			{
				G_PROFILE("Simulation");
				RunService->PreSimulation->Fire(deltaTime);
				Workspace->CurrentCamera->Step(deltaTime);
				RunService->PostSimulation->Fire(deltaTime);
			}

			{
				G_PROFILE("PreRender");
				RunService->PreRender->Fire(deltaTime);
			}

			{
				G_PROFILE("Draw");
				Renderer->Draw({
					.WorldRoot = WorldRoot,
					.Camera = Workspace->CurrentCamera,
				});
			}

			{
				G_PROFILE("Scripts");
				Script->Step();
			}
		}

		G_PROFILE_FRAME();

		LastTick = CurrentTick;
	}
}
