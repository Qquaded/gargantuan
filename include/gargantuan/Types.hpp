#pragma once

#include <any>
#include <lua.h>
#include <string_view>

namespace gargantuan::Types {

template <typename T> struct Type {
  public:
    std::string_view Name;
    std::string_view LuauTypeAlias;

    std::function<bool(lua_State *, int)> IsStackValue;
    std::function<T(lua_State *, int)> FromStackValue;
    std::function<void(lua_State *, const T)> PushStackValue;
};

struct AnyType {
  public:
    virtual ~AnyType() = default;
    AnyType() = default;
    AnyType(const AnyType &) = default;
    AnyType &operator=(const AnyType &) = default;
    AnyType(AnyType &&) noexcept = default;
    AnyType &operator=(AnyType &&) noexcept = default;

    std::string_view Name;
    std::string_view LuauTypeAlias;

    std::function<bool(lua_State *, int)> IsStackValue;
    std::function<std::any(lua_State *, int)> FromStackValue;
    std::function<void(lua_State *, const std::any &)> PushStackValue;

    template <typename T>
    AnyType(const Type<T> &specificType)
        : Name(specificType.Name), LuauTypeAlias(specificType.LuauTypeAlias), IsStackValue(specificType.IsStackValue) {
        FromStackValue = [specificType](lua_State *L, int idx) -> std::any {
            return std::any(specificType.FromStackValue(L, idx));
        };

        PushStackValue = [specificType](lua_State *L, std::any value) {
            specificType.PushStackValue(L, std::any_cast<T>(value));
        };
    };
};

static const Type<bool> BOOLEAN = {
    .Name = "Boolean",
    .LuauTypeAlias = "boolean",
    .IsStackValue = [](lua_State *L, int idx) -> bool { return lua_isboolean(L, idx); },
    .FromStackValue = [](lua_State *L, int idx) -> bool { return lua_toboolean(L, idx); },
    .PushStackValue = [](lua_State *L, const bool &value) { lua_pushboolean(L, value); }
};

static const Type<float> FLOAT = {
    .Name = "Float",
    .LuauTypeAlias = "number",
    .IsStackValue = [](lua_State *L, int idx) -> bool { return lua_isnumber(L, idx); },
    .FromStackValue = [](lua_State *L, int idx) -> float { return lua_tonumber(L, idx); },
    .PushStackValue = [](lua_State *L, const float &value) { lua_pushnumber(L, value); }
};

static const Type<std::string_view> STRING = {
    .Name = "String",
    .LuauTypeAlias = "string",
    .IsStackValue = [](lua_State *L, int idx) -> bool { return lua_isstring(L, idx); },
    .FromStackValue = [](lua_State *L, int idx) -> std::string_view {
        size_t len;
        const char *str = lua_tolstring(L, idx, &len);
        return std::string_view(str, len);
    },
    .PushStackValue = [](lua_State *L, const std::string_view &value) -> void {
        lua_pushlstring(L, value.data(), value.size());
    },
};

// NOTE: not implementing IsStackValue and FromStackValue for now
template <typename T> Type<std::vector<T>> Array(Type<T> elementType, std::string_view name = "Array") {
    return {
        .Name = name,
        .LuauTypeAlias = "Array", // std::string("{ ") + elementType.LuauTypeAlias + " }",
        .PushStackValue = [elementType](lua_State *L, const std::vector<T> &value) {
            size_t len = value.size();
            lua_createtable(L, len, 0);
            int tableIdx = lua_gettop(L);
            for (size_t i = 0; i < len; ++i) {
                elementType.PushStackValue(L, value[i]);
                lua_rawseti(L, tableIdx, i + 1);
            }
        },
    };
}

} // namespace gargantuan::Types
