#pragma once

#include <any>
#include <lua.h>
#include <string_view>

namespace gargantuan::scripting {

template <typename T> struct ScriptType {
  public:
    std::string_view Name;
    std::string_view LuauTypeAlias;

    bool (*IsStackValue)(lua_State *L, int idx);
    T (*FromStackValue)(lua_State *L, int idx);
    void (*PushStackValue)(lua_State *L, T value);
};

struct AnyScriptType {
  public:
    virtual ~AnyScriptType() = default;

    std::string_view Name;
    std::string_view LuauTypeAlias;

    std::function<bool(lua_State *, int)> IsStackValue;
    std::function<std::any(lua_State *, int)> FromStackValue;
    std::function<void(lua_State *, std::any)> PushStackValue;

    template <typename T>
    AnyScriptType(const ScriptType<T> &specificType)
        : Name(specificType.Name), LuauTypeAlias(specificType.LuauTypeAlias), IsStackValue(specificType.IsStackValue) {
        FromStackValue = [specificType](lua_State *L, int idx) -> std::any {
            return std::any(specificType.FromStackValue(L, idx));
        };

        PushStackValue = [specificType](lua_State *L, std::any value) {
            specificType.PushStackValue(L, std::any_cast<T>(value));
        };
    };
};

static const ScriptType<bool> TYPE_BOOLEAN = {
    .Name = "Boolean",
    .LuauTypeAlias = "boolean",
    .IsStackValue = [](lua_State *L, int idx) -> bool { return lua_isboolean(L, idx); },
    .FromStackValue = [](lua_State *L, int idx) -> bool { return lua_toboolean(L, idx); },
    .PushStackValue = [](lua_State *L, bool value) { lua_pushboolean(L, value); }
};

static const ScriptType<float> TYPE_FLOAT = {
    .Name = "Float",
    .LuauTypeAlias = "number",
    .IsStackValue = [](lua_State *L, int idx) -> bool { return lua_isnumber(L, idx); },
    .FromStackValue = [](lua_State *L, int idx) -> float { return lua_tonumber(L, idx); },
    .PushStackValue = [](lua_State *L, float value) { lua_pushnumber(L, value); }
};

static const ScriptType<std::string_view> TYPE_STRING = {
    .Name = "String",
    .LuauTypeAlias = "string",
    .IsStackValue = [](lua_State *L, int idx) -> bool { return lua_isstring(L, idx); },
    .FromStackValue = [](lua_State *L, int idx) -> std::string_view { return lua_tostring(L, idx); },
    .PushStackValue = [](lua_State *L, std::string_view value) { lua_pushstring(L, value.data()); }
};

} // namespace gargantuan::scripting
