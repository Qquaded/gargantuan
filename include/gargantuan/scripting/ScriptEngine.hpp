#pragma once

#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/scripting/ThreadEngine.hpp"

#include <Luau/Compiler.h>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <memory>
#include <tuple>

namespace gargantuan {
	int OpenLibBase(lua_State *L);
	int OpenLibTask(lua_State *L, ThreadEngine *threadEngine);

	int OpenLibAxes(lua_State *L);
	int OpenLibCFrame(lua_State *L);
	int OpenLibColor3(lua_State *L);
	int OpenLibColorSequence(lua_State *L);
	int OpenLibColorSequenceKeypoint(lua_State *L);
	int OpenLibEnum(lua_State *L);
	int OpenLibFaces(lua_State *L);
	int OpenLibFont(lua_State *L);
	int OpenLibInstance(lua_State *L);
	int OpenLibNumberRange(lua_State *L);
	int OpenLibNumberSequence(lua_State *L);
	int OpenLibNumberSequenceKeypoint(lua_State *L);
	int OpenLibRandom(lua_State *L);
	int OpenLibRaycastParams(lua_State *L);
	int OpenLibRect(lua_State *L);
	int OpenLibRegion3(lua_State *L);
	int OpenLibSignal(lua_State *L);
	int OpenLibTweenInfo(lua_State *L);
	int OpenLibUDim(lua_State *L);
	int OpenLibUDim2(lua_State *L);
	int OpenLibVector2(lua_State *L);
	int OpenLibVector3(lua_State *L);

	class ScriptEngine {
	  public:
		ScriptEngine(std::shared_ptr<DataModel> game);
		~ScriptEngine();

		lua_State *L = nullptr;
		ThreadEngine Threads;

		std::tuple<char *, size_t> CompileBytecode(std::string contents);
		std::tuple<char *, size_t> CompileBytecodeFromFile(const char *filepath);
		lua_State *ThreadFromBytecode(char *bytecode, size_t bytecodeSize, const char *chunkName);
		lua_State *ThreadFromBytecode(std::tuple<char *, size_t> &bytecodeResult, const char *chunkName);

		void Step();
		static void DumpStack(lua_State *L);
	};
} // namespace gargantuan
