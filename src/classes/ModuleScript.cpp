#pragma once

#include "gargantuan/classes/ModuleScript.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include <lua.h>

namespace gargantuan {
	G_INSTANCE_IMPL(ModuleScript, .Superclass = "LuaSourceContainer");

	ModuleScript::ModuleScript() {
		Destroying->Once([this](std::monostate _) {
			if (this->Thread) {
				lua_close(this->Thread);
				this->Thread = nullptr;
			};

			if (this->ThreadReference) {
				auto mainState = lua_mainthread(this->Thread);
				if (!mainState) return;
				lua_unref(mainState, this->ThreadReference);
				this->ThreadReference = LUA_NOREF;
			};
		});
	}

	ModuleScriptStatus ModuleScript::Step(lua_State *L) {
		if (Status == ModuleScriptStatus::Error || Status == ModuleScriptStatus::Success) return Status;
		Status = ModuleScriptStatus::Running;

		if (!Thread) {
			Thread = lua_newthread(L);

			auto loadError = LoadIntoState(Thread);
			if (loadError.has_value()) {
				Status = ModuleScriptStatus::Error;
				ErrorMessage = loadError.value();
				return Status;
			}

			lua_pushthread(Thread);
			lua_xmove(Thread, L, 1);
			ThreadReference = lua_ref(L, 1);
			// lua_ref keeps the value on the stack
			lua_pop(L, 1);
		}

		auto status = lua_resume(Thread, L, 0);
		switch (status) {
		case LUA_YIELD:
		case LUA_BREAK:
			break;

		case LUA_OK:
			if (lua_gettop(Thread) == 0) {
				Status = ModuleScriptStatus::Error;
				ErrorMessage = "ModuleScript did not return a value";
				break;
			}

			Status = ModuleScriptStatus::Success;

			lua_xmove(Thread, L, -1);
			ReturnReference = lua_ref(L, -1);
			lua_pop(L, -1);

			break;

		default:
			Status = ModuleScriptStatus::Error;
			ErrorMessage = lua_tostring(L, -1);
			break;
		}

		return Status;
	}
}
