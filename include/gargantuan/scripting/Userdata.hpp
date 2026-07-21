#pragma once

#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <lua.h>
#include <lualib.h>
#include <string_view>
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

        template <auto MethodPointer, typename Returns, typename... Arguments>
        static Method Wrap(Returns (Class::*)(Arguments...)) {
            return {[](lua_State *L, Class *instance) -> int {
                return WrappedCall<MethodPointer, Arguments...>(L, instance, std::index_sequence_for<Arguments...>{});
            }};
        }

        template <auto MethodPointer, typename R, typename... Arguments>
        static Method Wrap(R (Class::*)(Arguments...) const) {
            return {[](lua_State *L, Class *instance) -> int {
                return WrappedCall<MethodPointer, Arguments...>(L, instance, std::index_sequence_for<Arguments...>{});
            }};
        }

        template <auto MethodPointer> static Method Wrap() { return Wrap<MethodPointer>(MethodPointer); }

      private:
        template <auto MethodPointer, typename... Arguments, std::size_t... Indices>
        static int WrappedCall(lua_State *L, Class *instance, std::index_sequence<Indices...>) {
            using Ret = std::invoke_result_t<decltype(MethodPointer), Class *, std::decay_t<Arguments>...>;

            if constexpr (std::is_void_v<Ret>) {
                std::invoke(MethodPointer, instance, StackValue<std::decay_t<Arguments>>::From(L, Indices + 2)...);
                return 0;
            } else {
                auto &&res =
                    std::invoke(MethodPointer, instance, StackValue<std::decay_t<Arguments>>::From(L, Indices + 2)...);
                StackValue<std::decay_t<Ret>>::Push(L, std::forward<decltype(res)>(res));
                return 1;
            }
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

        if (auto it = Class::USERDATA_PROPERTIES.find(key); it != Class::USERDATA_PROPERTIES.end()) {
            const Property &property = it->second;
            if (property.Read) {
                property.Read(L, instance);
                return 1;
            }
            return 0;
        }

        return 0;
    };

    static int UserdataNewIndex(lua_State *L) {
        Class *instance = fromStackValue(L, 1);
        const char *key = luaL_checkstring(L, 2);

        if (!key || !instance) {
            return 0;
        }

        if (auto it = Class::USERDATA_PROPERTIES.find(key); it != Class::USERDATA_PROPERTIES.end()) {
            const Property &property = it->second;
            if (property.Write) {
                property.Write(L, instance);
            } else {
                luaL_error(L, "%s is read-only", key);
            }
            return 0;
        }

        return 0;
    };

    static int UserdataNamecall(lua_State *L) {
        Class *instance = fromStackValue(L, 1);
        const char *key = lua_namecallatom(L, nullptr);
        if (!key || !instance) {
            return 0;
        }

        if (auto it = Class::USERDATA_METHODS.find(key); it != Class::USERDATA_METHODS.end()) {
            const Method &method = it->second;
            return method.Call(L, instance);
        }

        return 0;
    };

    static int UserdataTostring(lua_State *L) {
        lua_pushstring(L, Class::USERDATA_TYPE.data());
        return 1;
    };

    static void CreateUserdataMetatable(lua_State *L) {
        lua_createtable(L, 0, 5);

        lua_pushstring(L, Class::USERDATA_TYPE.data());
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
        lua_setuserdatametatable(L, (int)Class::USERDATA_TAG);
    };

  private:
    template <typename T, typename = std::void_t<>> struct HasGetter : std::false_type {};
    template <typename T> struct HasGetter<T, std::void_t<decltype(std::declval<T>().get())>> : std::true_type {};

    static Class *fromStackValue(lua_State *L, int idx) {
        StoredAs *instancePointer = static_cast<StoredAs *>(lua_touserdatatagged(L, idx, (int)Class::USERDATA_TAG));
        if (!instancePointer) {
            return nullptr;
        };

        Class *instance = nullptr;
        if constexpr (std::is_pointer_v<StoredAs>) {
            instance = *instancePointer;
        } else if constexpr (HasGetter<StoredAs>::value) {
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
        StoredAs *userdata =
            static_cast<StoredAs *>(lua_touserdatatagged(L, idx, (int)Userdata<Class, StoredAs>::USERDATA_TAG));
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
    [](lua_State *L, void *rawInstance) -> int {                                                                       \
        auto *instance = static_cast<classType *>(rawInstance);                                                        \
        ::gargantuan::StackValue<valueType>::Push(L, instance->propertyName);                                          \
        return 1;                                                                                                      \
    }

#define USERDATA_WRITEONLY_PROP_IMPL(classType, propertyName, valueType)                                               \
    [](lua_State *L, void *rawInstance) -> int {                                                                       \
        auto *instance = static_cast<classType *>(rawInstance);                                                        \
        valueType value = ::gargantuan::StackValue<valueType>::From(L, -1);                                            \
        instance->propertyName = value;                                                                                \
        return 0;                                                                                                      \
    }

#define USERDATA_READONLY_PROP(classType, propertyName, valueType)                                                     \
    {                                                                                                                  \
        #propertyName, {                                                                                               \
            [](lua_State *L, auto *inst) -> int {                                                                      \
                return USERDATA_READONLY_PROP_IMPL(classType, propertyName, valueType)(L, inst);                       \
            },                                                                                                         \
                nullptr                                                                                                \
        }                                                                                                              \
    }

#define USERDATA_WRITEONLY_PROP(classType, propertyName, valueType)                                                    \
    {                                                                                                                  \
        #propertyName, {                                                                                               \
            nullptr, [](lua_State *L, auto *inst) -> int {                                                             \
                return USERDATA_WRITEONLY_PROP_IMPL(classType, propertyName, valueType)(L, inst);                      \
            }                                                                                                          \
        }                                                                                                              \
    }

#define USERDATA_READWRITE_PROP(classType, propertyName, valueType)                                                    \
    {                                                                                                                  \
        #propertyName, {                                                                                               \
            [](lua_State *L, auto *inst) -> int {                                                                      \
                return USERDATA_READONLY_PROP_IMPL(classType, propertyName, valueType)(L, inst);                       \
            },                                                                                                         \
                [](lua_State *L, auto *inst) -> int {                                                                  \
                    return USERDATA_WRITEONLY_PROP_IMPL(classType, propertyName, valueType)(L, inst);                  \
                }                                                                                                      \
        }                                                                                                              \
    }

#define USERDATA_STACKVALUE_WITH_STORED(classType, storedType)                                                         \
    template <> struct StackValue<storedType> {                                                                        \
        static inline std::string_view ReflectedTypedef() {                                                            \
            return StackValue<Userdata<classType, storedType>>::ReflectedTypedef();                                    \
        };                                                                                                             \
                                                                                                                       \
        static bool Is(lua_State *L, int idx) { return StackValue<Userdata<classType, storedType>>::Is(L, idx); };     \
                                                                                                                       \
        static storedType From(lua_State *L, int idx) {                                                                \
            return StackValue<Userdata<classType, storedType>>::From(L, idx);                                          \
        };                                                                                                             \
                                                                                                                       \
        static void Push(lua_State *L, storedType value) {                                                             \
            return StackValue<Userdata<classType, storedType>>::Push(L, value);                                        \
        };                                                                                                             \
    };

#define USERDATA_STACKVALUE(classType) USERDATA_STACKVALUE_WITH_STORED(classType, classType)

} // namespace gargantuan
