#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/Types.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/instances/ClassRegistry.hpp"
#include "gargantuan/scripting/Runtime.hpp"
#include "gargantuan/scripting/UserdataTags.hpp"

#include <SDL3/SDL.h>
#include <cstring>
#include <lua.h>
#include <lualib.h>
#include <vector>

namespace gargantuan::scripting::runtime {

int instance_gc(lua_State *L) {
    auto *ptr =
        static_cast<std::shared_ptr<instances::Instance> *>(lua_touserdatatagged(L, 1, (int)UserdataTags::Instance));

    if (ptr) {
        ptr->~shared_ptr();
    }

    return 0;
}

int instance_index(lua_State *L) {
    std::shared_ptr<instances::Instance> instance = Types::INSTANCE.FromStackValue(L, -2);
    if (!instance) {
        return 0;
    };

    auto *classDefinition = instances::ClassRegistry::GetDefinition(instance.get());
    if (!classDefinition) {
        return 0;
    }

    auto properties = instances::ClassRegistry::GetProperties(classDefinition);

    const char *keyPtr = luaL_checkstring(L, 2);
    if (!keyPtr) {
        return 0;
    }
    std::string_view key(keyPtr);

    auto property = properties.find(key);
    if (property != properties.end()) {
        auto &propertyDefinition = property->second;
        if (propertyDefinition.Read) {
            propertyDefinition.Type.PushStackValue(L, propertyDefinition.Read(instance.get()));
            return 1;
        }
    } else {
        auto child = instance->FindFirstChild(key);
        if (child) {
            Types::INSTANCE.PushStackValue(L, child);
            return 1;
        }
    }

    luaL_error(L, "Unknown member %s of class %s", key.data(), classDefinition->Name.data());

    return 0;
}

int instance_newindex(lua_State *L) {
    std::shared_ptr<instances::Instance> instance = Types::INSTANCE.FromStackValue(L, -3);
    if (!instance) {
        return 0;
    };

    auto *classDefinition = instances::ClassRegistry::GetDefinition(instance.get());
    if (!classDefinition) {
        return 0;
    }

    auto properties = instances::ClassRegistry::GetProperties(classDefinition);

    const char *keyPtr = luaL_checkstring(L, 2);
    if (!keyPtr) {
        return 0;
    }
    std::string_view key(keyPtr);

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
        propertyDefinition.Write(instance.get(), value);
        return 0;
    }

    return 0;
}

int instance_namecall(lua_State *L) {
    std::shared_ptr<instances::Instance> instance = Types::INSTANCE.FromStackValue(L, 1);
    if (!instance) {
        luaL_error(L, "Instance already destroyed");
        return 0;
    };

    const char *key = lua_namecallatom(L, nullptr);
    auto classDefinition = instances::ClassRegistry::GetDefinition(instance.get());
    auto methods = instances::ClassRegistry::GetMethods(classDefinition);

    auto methodIt = methods.find(key);
    if (methodIt == methods.end()) {
        luaL_error(L, "%s is not a valid method of %s", key, classDefinition->Name.data());
        return 0;
    }

    auto &methodDefinition = methodIt->second;

    std::vector<std::any> arguments;
    arguments.reserve(methodDefinition.Arguments.size());
    for (int i = 0; i < methodDefinition.Arguments.size(); ++i) {
        int stackIndex = i + 2;
        const auto &argumentDefinition = methodDefinition.Arguments[i];

        if (!argumentDefinition.Type.IsStackValue(L, stackIndex)) {
            luaL_error(
                L, "Argument %d to %s must be of type %s", static_cast<int>(i + 1), key,
                argumentDefinition.Type.LuauTypeAlias.data()
            );
            return 0;
        }

        arguments.push_back(argumentDefinition.Type.FromStackValue(L, stackIndex));
    }

    std::vector<std::any> returnValues = methodDefinition.Invoke(instance.get(), std::move(arguments));

    auto returnCount = methodDefinition.Returns.size();
    for (int i = 0; i < returnCount; i++) {
        const auto &returnDefinition = methodDefinition.Returns[i];
        returnDefinition.Type.PushStackValue(L, returnValues[i]);
    }

    return returnCount;
}

int instance_tostring(lua_State *L) {
    std::shared_ptr<instances::Instance> instance = Types::INSTANCE.FromStackValue(L, -1);
    if (!instance) {
        return 0;
    };

    Types::STRING.PushStackValue(L, instance->GetFullName());
    return 1;
}

void instance_createmetatable(lua_State *L) {
    lua_createtable(L, 0, 6);

    lua_pushliteral(L, "Instance");
    lua_setfield(L, -2, "__type");

    lua_pushcfunction(L, instance_index, "Instance.__index");
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, instance_newindex, "Instance.__newindex");
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, instance_tostring, "Instance.__tostring");
    lua_setfield(L, -2, "__tostring");

    lua_pushcfunction(L, instance_namecall, "Instance.__namecall");
    lua_setfield(L, -2, "__namecall");

    lua_pushcfunction(L, instance_gc, "Instance.__gc");
    lua_setfield(L, -2, "__gc");

    lua_setuserdatametatable(L, (int)UserdataTags::Instance);
}

int libInstance_new(lua_State *L) {
    std::string_view className = luaL_checkstring(L, -1);

    auto classDefinition = instances::ClassRegistry::GetDefinitionByName(className);
    if (!classDefinition) {
        luaL_error(L, "Unknown instance class %s", className.data());
        return 0;
    }

    auto &constructor = classDefinition->Constructor;
    if (!constructor) {
        luaL_error(L, "Cannot construct instance of class %s", className.data());
        return 0;
    }

    auto instance = classDefinition->Constructor();
    Types::INSTANCE.PushStackValue(L, instance);
    return 1;
}

luaL_Reg libInstance[] = {
    {"new", libInstance_new},
    {nullptr, nullptr},
};

int OpenInstance(lua_State *L) {
    instance_createmetatable(L);
    luaL_register(L, "Instance", libInstance);
    return 0;
}

} // namespace gargantuan::scripting::runtime
