#include "gargantuan/scripting/Runtime.hpp"

namespace gargantuan::scripting::runtime {

int OpenEnvironment(lua_State *L) {
    lua_newtable(L);
    lua_pushstring(L, "gargantuan");
    lua_setfield(L, -2, "name");
    lua_pushstring(L, "https://gargantuan.teamfireworks.org/");
    lua_setfield(L, -2, "url");
    lua_setreadonly(L, -1, true);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "_RUNTIME");

    return 1;
}

} // namespace gargantuan::scripting::runtime
