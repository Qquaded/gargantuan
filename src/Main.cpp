#include "gargantuan/Engine.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/Project.hpp"
#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/datatypes/Vector2.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <argparse/argparse.hpp>
#include <exception>
#include <magic_enum/magic_enum.hpp>
#include <memory>

int main(int argc, char *argv[]) {
	SDL_SetLogOutputFunction(gargantuan::OutputLog, nullptr);

	argparse::ArgumentParser program("gargantuan");
	program.add_description("An independent game engine for Roblox developers");
	program.add_argument("--project").help("path of a project directory to be loaded").default_value("-");
	program.add_argument("--script").help("path of a Luau script to be loaded").default_value("-");
	program.add_argument("--headless").flag().help("whether to disable the renderer");

	try {
		program.parse_args(argc, argv);
	} catch (std::exception &e) {
		G_LOG_CRITICAL("%s", e.what());
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	std::atexit(SDL_Quit);

	// Constructing the renderer
	gargantuan::Vector2 viewportSize(720, 540);
	gargantuan::BaseRenderer *renderer = nullptr;
	if (program.is_used("--headless")) {
		renderer = new gargantuan::HeadlessRenderer(viewportSize);
	} else {
		try {
			renderer = new gargantuan::SDLRenderer(viewportSize);
		} catch (std::exception &e) {
			G_LOG_CRITICAL("Failed to construct SDL3 renderer: %s", e.what());
			return 1;
		}
	}

	// Constructing the engine
	gargantuan::Engine *engine = nullptr;

	if (program.is_used("--project")) {
		// Projects
		auto path = program.get<std::string>("--project");
		try {
			auto project = gargantuan::Project::fromExisting(path);
			auto game = project.DeserializeGame();
			engine = new gargantuan::Engine(game, renderer);
		} catch (std::exception &e) {
			G_LOG_CRITICAL("Failed to deserialize project %s: %s", path.c_str(), e.what());
			return 1;
		}
	} else if (program.is_used("--script")) {
		// Scripts
		// TODO: Wrap scripts into a LuaSourceContainer, then provide a script
		// global like Roblox
		auto path = program.get<std::string>("--script");

		SDL_PathInfo pathInfo;
		if (!SDL_GetPathInfo(path.c_str(), &pathInfo)) {
			G_LOG_CRITICAL("Failed to read script %s: %s", path.c_str(), SDL_GetError());
			return 1;
		}

		if (pathInfo.type != SDL_PATHTYPE_FILE) {
			G_LOG_CRITICAL(
				"Expected script %s to be a file, got a %s", path.c_str(), magic_enum::enum_name(pathInfo.type).data()
			);
			return 1;
		}

		auto game = std::make_shared<gargantuan::DataModel>();
		engine = new gargantuan::Engine(game, renderer);

		try {
			auto bytecodeResult = engine->Script->CompileBytecodeFromFile(path.c_str());
			auto scriptThread = engine->Script->ThreadFromBytecode(bytecodeResult, path.c_str());
			engine->Script->Threads.QueueDeferredTask(scriptThread, 0);
		} catch (std::exception &e) {
			G_LOG_CRITICAL("Failed to load script %s: %s", path.c_str(), e.what());
			return 1;
		}
	} else {
		G_LOG_CRITICAL("Missing --project or --script to load");
		return 1;
	}

	G_LOG_INFO("Starting engine loop");
	try {
		while (engine->IsRunning) {
			engine->Step();
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	engine->Destroy();
	return 0;
}
