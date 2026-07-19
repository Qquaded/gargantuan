#include "gargantuan/scripting/Runtime.hpp"

#include <lualib.h>

namespace gargantuan::scripting::runtime {

static const luaL_Reg RUNTIME_MODULES[] = {
    {"", OpenEnvironment},
    {"Instance", OpenInstance},
};

void Open(lua_State *L) {
    for (luaL_Reg module : RUNTIME_MODULES) {
        lua_pushcfunction(L, module.func, NULL);
        lua_pushstring(L, module.name);
        lua_call(L, 1, 0);
    }
}

} // namespace gargantuan::scripting::runtime
