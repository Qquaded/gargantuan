#pragma once

#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <lua.h>
#include <string>

namespace gargantuan {
	class ModuleScript : public LuaSourceContainer {
		G_INSTANCE_DECL(ModuleScript);

		enum class Status : int { Idle, Running, Error, Success };
		ModuleScript::Status Status = ModuleScript::Status::Idle;
		std::string ErrorMessage;
		int ReturnReference = LUA_NOREF;
	};
}
