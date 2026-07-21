#pragma once

#include <Luau/Compiler.h>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>

namespace gargantuan {
class ScriptEngine {
  public:
    ScriptEngine();
    ~ScriptEngine();

    lua_State *L;

    lua_State *testbedThread;
    void Step();

  private:
    bool testbedFinished = false;
};
} // namespace gargantuan
