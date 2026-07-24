#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"

#include <lualib.h>

namespace gargantuan {

int LibCFrame_new(lua_State *L) {
    if (StackValue<glm::vec3>::Is(L, -1)) {
        StackValue<CFrame>::Push(L, CFrame(StackValue<glm::vec3>::From(L, -1)));
        return 1;
    }

    luaL_error(L, "unsupported constructor");
    return 0;
}

luaL_Reg LibCFrame[] = {
    {"new", LibCFrame_new},
    {nullptr, nullptr},
};

int OpenLibCFrame(lua_State *L) {
    luaL_register(L, "CFrame", LibCFrame);
    return 0;
}

} // namespace gargantuan
