#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include <lualib.h>

namespace gargantuan {

int LibVector3_new(lua_State *L) {
    Vector3 vec = {
        (float)luaL_opt(L, luaL_checknumber, 1, 0.0f),
        (float)luaL_opt(L, luaL_checknumber, 2, 0.0f),
        (float)luaL_opt(L, luaL_checknumber, 3, 0.0f),
    };
    StackValue<Vector3>::Push(L, vec);
    return 1;
}

luaL_Reg LibVector3[] = {
    {"new", LibVector3_new},
    {nullptr, nullptr},
};

int OpenLibVector3(lua_State *L) {
    luaL_register(L, "Vector3", LibVector3);
    return 0;
}

} // namespace gargantuan
