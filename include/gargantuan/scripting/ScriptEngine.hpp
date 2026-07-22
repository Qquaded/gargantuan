#pragma once

#include <Luau/Compiler.h>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>

namespace gargantuan {

int OpenLibBase(lua_State *L);
int OpenLibCFrame(lua_State *L);
int OpenLibColor3(lua_State *L);
int OpenLibVector3(lua_State *L);
int OpenLibInstance(lua_State *L);

class ScriptEngine {
  public:
    ScriptEngine();
    ~ScriptEngine();

    lua_State *L;

    void Step();

  private:
    lua_State *testbedThread;
    bool testbedFinished = false;
    void CreateTestbedThread();
};

} // namespace gargantuan
