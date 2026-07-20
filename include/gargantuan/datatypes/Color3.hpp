#pragma once

#include "gargantuan/Types.hpp"
#include "gargantuan/scripting/UserdataTags.hpp"

#include <glm/glm.hpp>
#include <lua.h>

namespace gargantuan::datatypes {

class Color3 {
  public:
    Color3(float r, float g, float b);
    Color3();

    float R;
    float G;
    float B;

    static Color3 fromRGB(float r, float g, float b);
    static Color3 fromHSV(float h, float s, float v);

    operator glm::vec3() const { return {R, G, B}; }
};

} // namespace gargantuan::datatypes

namespace gargantuan::Types {

static const Type<datatypes::Color3> COLOR3 = {
    .Name = "Color3",
    .LuauTypeAlias = "Color3",
    .IsStackValue = [](lua_State *L, int idx) -> bool {
        return lua_userdatatag(L, idx) == (int)scripting::UserdataTags::Color3;
    },
    .FromStackValue = [](lua_State *L, int idx) -> datatypes::Color3 {
        auto *userdata = static_cast<datatypes ::Color3 *>(lua_touserdata(L, idx));
        datatypes ::Color3 value = *userdata;
        return value;
    },
    .PushStackValue = [](lua_State *L, datatypes ::Color3 value) -> void {
        auto *userdata = static_cast<datatypes ::Color3 *>(
            lua_newuserdatataggedwithmetatable(L, sizeof(datatypes::Color3), (int)scripting::UserdataTags::Color3)
        );
        *userdata = value;
    },
};

} // namespace gargantuan::Types
