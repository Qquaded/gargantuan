#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/scripting/ThreadEngine.hpp"

#include <Luau/Compiler.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <cstdlib>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <stdexcept>

namespace gargantuan {

static const luaL_Reg SCRIPT_LIBS[] = {
    {"", OpenLibBase},

    {"CFrame", OpenLibCFrame},
    {"Color3", OpenLibColor3},
    {"Vector3", OpenLibVector3},

    {"Instance", OpenLibInstance},

    {nullptr, nullptr},
};

ScriptEngine::ScriptEngine() : L(luaL_newstate()), ThreadEngine(L) {
    if (L == nullptr) {
        throw std::runtime_error("Failed to instantiate Luau VM");
    }

    luaL_openlibs(L);
    OpenLibTask(L, &ThreadEngine);

    CFrame::CreateUserdataMetatable(L);
    Color3::CreateUserdataMetatable(L);
    Vector3::CreateUserdataMetatable(L);
    Instance::CreateUserdataMetatable(L);
    const luaL_Reg *lib = SCRIPT_LIBS;
    for (; lib->func; lib++) {
        lua_pushcfunction(L, lib->func, nullptr);
        lua_pushstring(L, lib->name);
        lua_call(L, 1, 0);
    }

    CreateTestbedThread();
}

void ScriptEngine::CreateTestbedThread() {
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
    std::free(bytecode);

    ThreadEngine.QueueDeferredTask(testbedThread, 0);
}

ScriptEngine::~ScriptEngine() {
    if (L) {
        lua_close(L);
        L = nullptr;
    }
}

void ScriptEngine::Step() { ThreadEngine.Step(); }

} // namespace gargantuan
