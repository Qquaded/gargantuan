#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/datatypes/Axes.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Color3.hpp"
#include "gargantuan/datatypes/Enum.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Random.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/datatypes/TweenInfo.hpp"
#include "gargantuan/datatypes/UDim.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/scripting/ThreadEngine.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <Luau/Common.h>
#include <Luau/Compiler.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <lua.h>
#include <luacode.h>
#include <lualib.h>
#include <magic_enum/magic_enum.hpp>
#include <stdexcept>

namespace gargantuan {
	struct Lib {
		std::string Label;
		std::function<void(lua_State *L)> Open = nullptr;
		std::function<void(lua_State *L)> CreateUserdataMetatable = nullptr;
	};

	static const Lib SCRIPT_LIBS[] = {
		{"Base", OpenLibBase},

		{"Axes", OpenLibAxes, Axes::CreateUserdataMetatable},
		{"CFrame", OpenLibCFrame, CFrame::CreateUserdataMetatable},
		{"Color3", OpenLibColor3, Color3::CreateUserdataMetatable},
		// {"ColorSequence", OpenLibColorSequence, ColorSequence::CreateUserdataMetatable},
		// {"ColorSequenceKeypoint", OpenLibColorSequenceKeypoint, ColorSequenceKeypoint::CreateUserdataMetatable},
		{"Enum", OpenLibEnum, Enum::CreateUserdataMetatable},
		{"EnumItem", nullptr, EnumItem::CreateUserdataMetatable},
		// {"Faces", OpenLibFaces, Faces::CreateUserdataMetatable},
		// {"Font", OpenLibFont, Font::CreateUserdataMetatable},
		{"Instance", OpenLibInstance, Instance::CreateUserdataMetatable},
		// {"NumberRange", OpenLibNumberRange, NumberRange::CreateUserdataMetatable},
		// {"NumberSequence", OpenLibNumberSequence, NumberSequence::CreateUserdataMetatable},
		// {"NumberSequenceKeypoint", OpenLibNumberSequenceKeypoint, NumberSequenceKeypoint::CreateUserdataMetatable},
		{"Random", OpenLibRandom, Random::CreateUserdataMetatable},
		// {"RaycastParams", OpenLibRaycastParams},
		// {"Rect", OpenLibRect, Rect::CreateUserdataMetatable},
		// {"Region3", OpenLibRegion3, Region3::CreateUserdataMetatable},
		{"UDim", OpenLibUDim, UDim::CreateUserdataMetatable},
		// {"UDim2", OpenLibUDim2, UDim2::CreateUserdataMetatable},
		{"Signal", OpenLibSignal, BaseSignal::CreateUserdataMetatable},
		{"SignalConnection", nullptr, SignalConnection::CreateUserdataMetatable},
		{"TweenInfo", OpenLibTweenInfo, TweenInfo::CreateUserdataMetatable},
		{"Vector2", OpenLibVector2, Vector2::CreateUserdataMetatable},
		{"Vector3", OpenLibVector3},
	};

	static thread_local lua_State *CurrentState = nullptr;

	ScriptEngine::ScriptEngine() : L(luaL_newstate()), ThreadEngine(L) {
		if (L == nullptr) {
			throw std::runtime_error("Failed to instantiate Luau VM");
		}

		CurrentState = L;
		Luau::assertHandler() = [](const char *expression, const char *file, int line, const char *function) -> int {
			if (CurrentState) ScriptEngine::DumpStack(CurrentState);
			SDL_Log("Luau assertion failed:\n\tExpression: %s\n\tIn: %s:%d in %s", expression, file, line, function);
			assert(false);
		};

		luaL_openlibs(L);
		OpenLibTask(L, &ThreadEngine);
		for (const auto &[name, open, metatable] : SCRIPT_LIBS) {
			SDL_Log("Opening library %s", name.c_str());
			if (metatable) metatable(L);
			if (open) open(L);
		}
		SDL_Log("ScriptEngine finished opening libraries");

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

	void ScriptEngine::DumpStack(lua_State *L) {
		int stackSize = lua_gettop(L);

		SDL_Log("Dumping Lua stack of %d values", stackSize);

		for (int i = stackSize; i >= 1; --i) {
			int type = lua_type(L, i);

			switch (type) {
			case LUA_TNIL:
				SDL_Log("[%d] Nil", i);
				break;
			case LUA_TBOOLEAN:
				SDL_Log("[%d] Boolean: %s", i, lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				SDL_Log("[%d] Number: %g", i, lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				SDL_Log("[%d] String: %s", i, lua_tostring(L, i));
				break;
			case LUA_TTABLE:
				SDL_Log("[%d] Table", i);
				break;
			case LUA_TFUNCTION:
				SDL_Log("[%d] Function", i);
				break;
			case LUA_TUSERDATA: {
				auto tag = lua_userdatatag(L, i);
				if (tag != -1 || tag != (int)UserdataTag::Invalid) {
					auto tagName = magic_enum::enum_name((UserdataTag)tag);
					SDL_Log("[%d] Userdata (%d)%s", i, tag, tagName.data());
				} else {
					SDL_Log("[%d] Userdata (Not tagged)", i);
				}
				break;
			}
			case LUA_TTHREAD:
				SDL_Log("[%d] Thread", i);
				break;
			case LUA_TLIGHTUSERDATA:
				SDL_Log("[%d] Light Userdata", i);
				break;
			default:
				SDL_Log("[%d] Unknown", i);
				break;
			}
		}
	}

	ScriptEngine::~ScriptEngine() {
		if (L) {
			lua_close(L);
			L = nullptr;
			CurrentState = nullptr;
		}
	}

	void ScriptEngine::Step() {
		ThreadEngine.Step();
	}
} // namespace gargantuan
