#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/scripting/Runtime.hpp"

#include <Luau/Compiler.h>
#include <SDL3/SDL.h>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <stdexcept>
#include <sys/stat.h>

namespace gargantuan::scripting {

ScriptEngine::ScriptEngine() : L(luaL_newstate()) {
    if (L == nullptr) {
        throw std::runtime_error("Failed to instantiate Luau VM");
    }

    luaL_openlibs(L);
    runtime::Open(L);

    testbedThread = lua_newthread(L);
    size_t fileSize;
    void *code = SDL_LoadFile("Testbed.luau", &fileSize);

    if (code == nullptr) {
        SDL_Log("Failed to load Testbed.luau");
        return;
    }

    std::string contents((char *)code, fileSize);
    SDL_free(code);

    size_t bytecodeSize;
    char *bytecode = luau_compile(contents.c_str(), contents.length(), nullptr, &bytecodeSize);

    luau_load(testbedThread, "Testbed", bytecode, bytecodeSize, 0);
}

ScriptEngine::~ScriptEngine() {
    if (L) {
        lua_close(L);
        L = nullptr;
    }
}

void ScriptEngine::Step() {
    if (testbedFinished)
        return;

    int status = lua_resume(testbedThread, nullptr, 0);
    switch (status) {
    case LUA_YIELD:
        break;
    case LUA_OK:
        testbedFinished = true;
        break;
    default:
        SDL_Log("Testbed error: %s", lua_tostring(testbedThread, -1));
        testbedFinished = true;
        break;
    }
}

} // namespace gargantuan::scripting
