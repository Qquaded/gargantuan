#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL.h>
#include <cstdlib>
#include <format>
#include <lua.h>
#include <optional>
#include <string>

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(LuaSourceContainer);

	std::optional<std::string> LuaSourceContainer::CompileBytecode(lua_CompileOptions *options) {
		if (BytecodeCompiled) return std::nullopt;

		auto rawBytecode = luau_compile(Source.c_str(), Source.length(), options, &BytecodeSize);
		if (rawBytecode == nullptr) return std::format("Failed to compile script chunk %s", ChunkName.c_str());

		Bytecode.assign(rawBytecode, rawBytecode + BytecodeSize);
		std::free(rawBytecode);
		return std::nullopt;
	};

	std::optional<std::string> LuaSourceContainer::LoadIntoState(lua_State *L) {
		if (!BytecodeCompiled) return "Bytecode must be compiled prior to LuaSourceContainer::LoadIntoState";

		StackValue<Instance::Pointer>::Push(Thread, shared_from_this());
		lua_setglobal(Thread, "script");

		luaL_sandbox(L);
		if (luau_load(Thread, ChunkName.c_str(), Bytecode.data(), BytecodeSize, 0) != LUA_OK) {
			return std::format("Failed to load %s: %s", ChunkName.c_str(), lua_tostring(Thread, -1));
		};

		return std::nullopt;
	}
}
