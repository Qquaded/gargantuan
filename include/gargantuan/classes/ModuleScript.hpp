#pragma once

#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <lua.h>
#include <string>

namespace gargantuan {
	enum class ModuleScriptStatus : int { Idle, Running, Error, Success };

	class ModuleScript : public LuaSourceContainer {
	  public:
		G_INSTANCE_DECL(ModuleScript);

		ModuleScript();

		ModuleScriptStatus Status = ModuleScriptStatus::Idle;
		std::string ErrorMessage;
		lua_State *Thread = nullptr;
		int ThreadReference = LUA_NOREF;
		int ReturnReference = LUA_NOREF;

		ModuleScriptStatus Step(lua_State *L);
	};
}
