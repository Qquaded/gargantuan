#pragma once

#include "gargantuan/datatypes/Instance.hpp"

#include <SDL3/SDL.h>
#include <lua.h>
#include <luacode.h>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace gargantuan {
	class LuaSourceContainer : public Instance {
	  public:
		G_INSTANCE_DECL(LuaSourceContainer);

		std::string ChunkName = GetFullName();
		std::string Source = "";

		bool BytecodeCompiled = false;
		std::vector<char> Bytecode;
		size_t BytecodeSize;
		lua_State *Thread;

		[[nodiscard]] std::optional<std::string> CompileBytecode(lua_CompileOptions *options);
		[[nodiscard]] std::optional<std::string> LoadIntoState(lua_State *L);
	};

	template <typename T>
		requires std::is_base_of_v<LuaSourceContainer, T> && (!std::is_same_v<LuaSourceContainer, T>)
	[[nodiscard]] static std::shared_ptr<T> ScriptFromFile(const char *filepath) {
		size_t fileSize;
		void *code = SDL_LoadFile(filepath, &fileSize);

		if (code == nullptr) {
			throw std::runtime_error(
				std::format("Failed to load %s from {}: {}", T::CLASS_DEFINITION.ClassName, filepath, SDL_GetError())
			);
		};

		std::string source((char *)code, fileSize);
		SDL_free(code);

		auto self = std::make_shared<T>();
		self->ChunkName = std::string(filepath);
		self->Source = source;
		return self;
	};
}
