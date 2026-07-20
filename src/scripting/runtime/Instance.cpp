#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/instances/ClassRegistry.hpp"
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
    auto *classDefinition = instances::ClassRegistry::GetDefinition(instance);
    if (!classDefinition) {
        return 0;
    }

    auto properties = instances::ClassRegistry::GetProperties(classDefinition);

    std::string_view key = luaL_checkstring(L, -1);
    auto property = properties.find(key);
    if (property != properties.end()) {
        auto &propertyDefinition = property->second;
        if (propertyDefinition.Read) {
            propertyDefinition.Type.PushStackValue(L, propertyDefinition.Read(instance));
            return 1;
        }
    }
    // ??

    return 0;
}

int InstanceNewIndex(lua_State *L) {
    auto **userdata = static_cast<instances::Instance **>(lua_touserdata(L, -3));
    if (!userdata || !*userdata)
        return 0;

    instances::Instance *instance = *userdata;
    auto *classDefinition = instances::ClassRegistry::GetDefinition(instance);
    if (!classDefinition) {
        return 0;
    }

    auto properties = instances::ClassRegistry::GetProperties(classDefinition);

    std::string_view key = luaL_checkstring(L, -2);
    auto property = properties.find(key);
    if (property != properties.end()) {
        auto &propertyDefinition = property->second;
        if (!propertyDefinition.Write) {
            luaL_error(L, "Cannot write to %s", propertyDefinition.Name.data());
        }

        if (!propertyDefinition.Type.IsStackValue(L, -1)) {
            luaL_error(
                L, "Cannot write to %s as it expects a value of type %s", propertyDefinition.Name.data(),
                propertyDefinition.Type.LuauTypeAlias.data()
            );
        };

        auto value = propertyDefinition.Type.FromStackValue(L, -1);
        propertyDefinition.Write(instance, value);
        return 0;
    }

    return 0;
}

int OpenInstance(lua_State *L) {
    lua_newtable(L);

    lua_pushcfunction(L, InstanceIndex, "Instance.__index");
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, InstanceNewIndex, "Instance.__newindex");
    lua_setfield(L, -2, "__newindex");

    lua_setuserdatametatable(L, (int)UserdataTags::Instance);

    return 0;
}

} // namespace gargantuan::scripting::runtime
