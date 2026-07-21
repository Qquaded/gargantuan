#pragma once

#include <lua.h>
#include <lualib.h>
#include <string>
#include <string_view>

namespace gargantuan {

template <typename T> struct StackValue {
    static inline std::string_view ReflectedTypedef();
    static bool Is(lua_State *L, int idx);
    static T From(lua_State *L, int idx);
    static void Push(lua_State *L, T value);

  private:
    // i hope NOBODY has to see the original error message

    template <typename Missing> struct GARGANTUAN_STACK_VALUE_IS_UNIMPLEMENTED_FOR {
        static constexpr bool value = false;
    };

    static_assert(GARGANTUAN_STACK_VALUE_IS_UNIMPLEMENTED_FOR<T>::value);
};

#define PRIMITIVE_STACK_VALUE(cppType, reflectedTypedef, isImpl, fromImpl, pushImpl)                                   \
    template <> struct StackValue<cppType> {                                                                           \
        static inline std::string_view ReflectedTypedef() { return reflectedTypedef; };                                \
        static bool Is(lua_State *L, int idx) { return isImpl(L, idx); };                                              \
        static cppType From(lua_State *L, int idx) { return fromImpl(L, idx); };                                       \
        static void Push(lua_State *L, cppType value) { return pushImpl(L, value); };                                  \
    };

#define STRING_STACK_VALUE(cppType)                                                                                    \
    template <> struct StackValue<cppType> {                                                                           \
        static inline std::string_view ReflectedTypedef() { return "string"; };                                        \
        static bool Is(lua_State *L, int idx) { return lua_isstring(L, idx); };                                        \
        static cppType From(lua_State *L, int idx) {                                                                   \
            size_t len;                                                                                                \
            const char *str = luaL_checklstring(L, idx, &len);                                                         \
            return {str, len};                                                                                         \
        };                                                                                                             \
        static void Push(lua_State *L, cppType value) { return lua_pushlstring(L, value.data(), value.length()); };    \
    };

PRIMITIVE_STACK_VALUE(float, "number", lua_isnumber, lua_tonumber, lua_pushnumber);
PRIMITIVE_STACK_VALUE(double, "number", lua_isnumber, lua_tonumber, lua_pushnumber);
PRIMITIVE_STACK_VALUE(bool, "boolean", lua_isboolean, lua_toboolean, lua_pushboolean);
PRIMITIVE_STACK_VALUE(const char *, "string", lua_isstring, lua_tostring, lua_pushstring);
STRING_STACK_VALUE(std::string);
STRING_STACK_VALUE(std::string_view);

template <typename... Types> struct StackValue<std::tuple<Types...>> {
    static inline std::string_view ReflectedTypedef() {
        // NOTE: tuples are probably not used for typedef generation right?
        return "any";
    };

    static void Push(lua_State *L, const std::tuple<Types...> &tuple) {
        std::apply([L](const auto &...args) { (StackValue<std::decay_t<decltype(args)>>::Push(L, args), ...); }, tuple);
    }

    static std::tuple<Types...> From(lua_State *L, int idx) {
        return FromImpl(L, idx, std::index_sequence_for<Types...>{});
    }

  private:
    template <std::size_t... Indices>
    static std::tuple<Types...> FromImpl(lua_State *L, int idx, std::index_sequence<Indices...>) {
        return std::make_tuple(StackValue<std::decay_t<Types>>::From(L, idx + static_cast<int>(Indices))...);
    }
};

} // namespace gargantuan
