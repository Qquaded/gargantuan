#include "gargantuan/datatypes/UDim2.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <lualib.h>

namespace gargantuan {
	int LibUDim2_new(lua_State *L) {
		int argumentCount = lua_gettop(L);

		if (argumentCount == 2 && StackValue<UDim>::Is(L, 1) && StackValue<UDim>::Is(L, 2)) {
			auto x = StackValue<UDim>::From(L, 1);
			auto y = StackValue<UDim>::From(L, 2);
			StackValue<UDim2>::Push(L, {x, y});
			return 1;
		} else {
			float xScale = luaL_optnumber(L, 1, 0.0f);
			int xOffset = luaL_optnumber(L, 2, 0);
			float yScale = luaL_optnumber(L, 3, 0.0f);
			int yOffset = luaL_optnumber(L, 4, 0);
			StackValue<UDim2>::Push(L, {xScale, xOffset, yScale, yOffset});
			return 1;
		}
	}

	int LibUDim2_fromOffset(lua_State *L) {
		int x = luaL_optnumber(L, 1, 0);
		int y = luaL_optnumber(L, 2, 0);
		StackValue<UDim2>::Push(L, {0.0f, x, 0.0f, y});
		return 1;
	}

	int LibUDim2_fromScale(lua_State *L) {
		float x = luaL_optnumber(L, 1, 0.0f);
		float y = luaL_optnumber(L, 2, 0.0f);
		StackValue<UDim2>::Push(L, {x, 0, y, 0});
		return 1;
	}

	luaL_Reg LibUDim2[]{
		{"new", LibUDim2_new},
		{"fromOffset", LibUDim2_fromOffset},
		{"fromScale", LibUDim2_fromScale},
		{nullptr, nullptr},
	};

	int OpenLibUDim2(lua_State *L) {
		luaL_register(L, "UDim2", LibUDim2);
		return 0;
	}
} // namespace gargantuan
