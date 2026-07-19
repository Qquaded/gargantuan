#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/scripting/Runtime.hpp"
#include "gargantuan/scripting/UserdataTags.hpp"

#include <SDL3/SDL.h>
#include <lua.h>
#include <lualib.h>

namespace gargantuan::scripting::runtime {

void PushInstance(lua_State *L, instances::Instance *instance) {
    auto **userdata = static_cast<instances::Instance **>(
        lua_newuserdatataggedwithmetatable(L, sizeof(instances::Instance *), (int)UserdataTags::Instance)
    );
    *userdata = instance;
}

int InstanceIndex(lua_State *L) {
    auto **userdata = static_cast<instances::Instance **>(lua_touserdata(L, -2));
    if (!userdata || !*userdata)
        return 0;
    instances::Instance *instance = *userdata;

    std::string key = luaL_checkstring(L, -1);
    SDL_Log("No fucking shot: Instance.%s", key.c_str());

    return 0;
}

int OpenInstance(lua_State *L) {
    lua_newtable(L);
    lua_pushcfunction(L, InstanceIndex, "Instance.__index");
    lua_setfield(L, -2, "__index");
    lua_setuserdatametatable(L, (int)UserdataTags::Instance);
    return 0;
}

} // namespace gargantuan::scripting::runtime
