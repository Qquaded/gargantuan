#pragma once

#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <lua.h>
#include <lualib.h>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace gargantuan {

template <typename Class, typename StoredAs = Class> class Userdata {
  public:
    typedef Userdata<Class, StoredAs> This;

    struct Property {
        int (*Read)(lua_State *L, Class *instance);
        int (*Write)(lua_State *L, Class *instance);
    };

    struct Method {
      public:
        int (*Call)(lua_State *L, Class *instance);

        template <typename... Arguments, typename Returns> static Method Wrap(Returns (Class::*method)(Arguments...)) {
            return {[method](lua_State *L, Class *instance) -> int {
                auto arguments = ExtractArguments<Arguments...>(L, std::index_sequence_for<Arguments...>{});
                if constexpr (std::is_void_v<Returns>) {
                    std::apply(
                        [instance, method](Arguments... unpackedArgs) { (instance->*method)(unpackedArgs...); },
                        arguments
                    );
                    return 0;
                } else {
                    Returns result = std::apply(
                        [instance, method](Arguments... unpackedArgs) { return (instance->*method)(unpackedArgs...); },
                        arguments
                    );
                    StackValue<Returns>::Push(L, result);
                    return 1;
                }
            }};
        }

      private:
        template <typename... Arguments, std::size_t... Indices>
        static std::tuple<Arguments...> ExtractArguments(lua_State *L, std::index_sequence<Indices...>) {
            // stack indices start from 2 to account for the self parameter
            return std::make_tuple(StackValue<std::decay_t<Arguments>>::From(L, Indices + 2)...);
        }
    };

    typedef std::unordered_map<std::string_view, Property> UserdataProperties;
    typedef std::unordered_map<std::string_view, Method> UserdataMethods;

    static inline UserdataTag USERDATA_TAG;
    static inline std::string_view USERDATA_TYPE;

    static inline UserdataProperties USERDATA_PROPERTIES;
    static inline UserdataMethods USERDATA_METHODS;

    static int UserdataIndex(lua_State *L) {
        Class *instance = fromStackValue(L, 1);
        const char *key = luaL_checkstring(L, 2);

        if (!key || !instance) {
            return 0;
        }

        if (auto it = USERDATA_PROPERTIES.find(key); it != USERDATA_PROPERTIES.end()) {
            const Property &property = it->second;
            return property.Read ? property.Read(L, instance) : 0;
        }

        return 0;
    };

    static int UserdataNewIndex(lua_State *L) {
        Class *instance = fromStackValue(L, 1);
        const char *key = luaL_checkstring(L, 2);

        if (!key || !instance) {
            return 0;
        }

        if (auto it = USERDATA_PROPERTIES.find(key); it != USERDATA_PROPERTIES.end()) {
            const Property &property = it->second;
            if (property.Write) {
                return property.Write(L, instance);
            } else {
                luaL_error(L, "%s is read-only", key);
                return 0;
            }
        }

        return 0;
    };

    static int UserdataNamecall(lua_State *L) {
        Class *instance = fromStackValue(L, 1);
        const char *key = lua_namecallatom(L, nullptr);
        if (!key || !instance) {
            return 0;
        }

        if (auto it = USERDATA_METHODS.find(key); it != USERDATA_METHODS.end()) {
            const Method &method = it->second;
            return method.Call(L, instance);
        }

        return 0;
    };

    static int UserdataTostring(lua_State *L) {
        lua_pushstring(L, USERDATA_TYPE.data());
        return 1;
    };

    static void CreateUserdataMetatable(lua_State *L) {
        lua_createtable(L, 0, 5);

        lua_pushstring(L, USERDATA_TYPE.data());
        lua_setfield(L, -2, "__type");

        lua_pushcfunction(L, Class::UserdataIndex, "__index");
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, Class::UserdataNewIndex, "__newindex");
        lua_setfield(L, -2, "__newindex");

        lua_pushcfunction(L, Class::UserdataNamecall, "__namecall");
        lua_setfield(L, -2, "__namecall");

        lua_pushcfunction(L, Class::UserdataTostring, "__tostring");
        lua_setfield(L, -2, "__tostring");

        lua_setreadonly(L, -1, true);
        lua_setuserdatametatable(L, (int)USERDATA_TAG);
    };

  private:
    // STRAIGHT BULLSHIT
    template <typename T, typename = std::void_t<>> struct has_get : std::false_type {};
    template <typename T> struct has_get<T, std::void_t<decltype(std::declval<T>().get())>> : std::true_type {};

    static Class *fromStackValue(lua_State *L, int idx) {
        StoredAs *instancePointer = static_cast<StoredAs *>(lua_touserdatatagged(L, idx, (int)USERDATA_TAG));
        if (!instancePointer) {
            return nullptr;
        };

        Class *instance = nullptr;
        if constexpr (std::is_pointer_v<StoredAs>) {
            instance = *instancePointer;
        } else if constexpr (has_get<StoredAs>::value) {
            instance = instancePointer->get();
        } else {
            instance = instancePointer;
        };

        return instance;
    }
};

template <typename Class, typename StoredAs> struct StackValue<Userdata<Class, StoredAs>> {
    static inline std::string_view ReflectedTypedef() { return Userdata<Class, StoredAs>::USERDATA_TYPE; };

    static bool Is(lua_State *L, int idx) {
        return lua_userdatatag(L, idx) == (int)Userdata<Class, StoredAs>::USERDATA_TAG;
    };

    static StoredAs From(lua_State *L, int idx) {
        StoredAs *userdata = static_cast<StoredAs *>(lua_touserdata(L, idx));
        return userdata ? *userdata : StoredAs{};
    };

    static void Push(lua_State *L, StoredAs value) {
        StoredAs *userdata = static_cast<StoredAs *>(
            lua_newuserdatataggedwithmetatable(L, sizeof(StoredAs), (int)Userdata<Class, StoredAs>::USERDATA_TAG)
        );
        new (userdata) StoredAs(value);
    };
};

#define USERDATA_READONLY_PROP_IMPL(classType, propertyName, valueType)                                                \
    [](lua_State *L, classType *instance) -> int {                                                                     \
        using gargantuan::StackValue;                                                                                  \
        StackValue<valueType>::Push(L, instance->propertyName);                                                        \
        return 1;                                                                                                      \
    }

#define USERDATA_WRITEONLY_PROP_IMPL(classType, propertyName, valueType)                                               \
    [](lua_State *L, classType *instance) -> int {                                                                     \
        using gargantuan::StackValue;                                                                                  \
        valueType value = StackValue<valueType>::From(L, -1);                                                          \
        instance->propertyName = value;                                                                                \
        return 0;                                                                                                      \
    }

#define USERDATA_READONLY_PROP(classType, propertyType, valueType)                                                     \
    {                                                                                                                  \
        #propertyType, { *USERDATA_READONLY_PROP_IMPL(classType, propertyType, valueType), nullptr }                   \
    }

#define USERDATA_WRITEONLY_PROP(classType, propertyType, valueType)                                                    \
    {                                                                                                                  \
        #propertyType, { nullptr, *USERDATA_WRITEONLY_PROP_IMPL(classType, propertyType, valueType) }                  \
    }

#define USERDATA_READWRITE_PROP(classType, propertyType, valueType)                                                    \
    {                                                                                                                  \
        #propertyType, {                                                                                               \
            *USERDATA_READONLY_PROP_IMPL(classType, propertyType, valueType),                                          \
                *USERDATA_WRITEONLY_PROP_IMPL(classType, propertyType, valueType)                                      \
        }                                                                                                              \
    }

} // namespace gargantuan
