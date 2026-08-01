#pragma once

#include "gargantuan/classes/LuaSourceContainer.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <lua.h>

namespace gargantuan {
	G_ENUM(RunContext, Client, Server);

	class Script : public LuaSourceContainer {
		G_INSTANCE_DECL(Script);

		bool Enabled = true;
		static void SetEnabled(Script *self);

		int ThreadReference = LUA_NOREF;
	};
}
