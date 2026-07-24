#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"
#include <SDL3/SDL_log.h>
#include <lua.h>
#include <memory>

namespace gargantuan {

std::string_view SignalConnection::GetUserdataType() { return "SignalConnection"; }
UserdataTag SignalConnection::GetUserdataTag() { return UserdataTag::SignalConnection; }

SignalConnection::SignalConnection(SignalConnection::CallbackType callback, lua_State *L, int callbackReference)
    : Callback(callback), L(L), CallbackReference(callbackReference) {};

const SignalConnection::UserdataProperties &SignalConnection::GetUserdataProperties() {
    static const SignalConnection::UserdataProperties PROPERTIES{
        USERDATA_READONLY_PROP(SignalConnection, Connected, bool),
    };
    return PROPERTIES;
}

const SignalConnection::UserdataMethods &SignalConnection::GetUserdataMethods() {
    static const SignalConnection::UserdataMethods METHODS{
        {"Disconnect", Method::Wrap<&SignalConnection::Disconnect>()},
        {"__gc", {&SignalConnection::LGarbageCollect}},
    };
    return METHODS;
}

void SignalConnection::Disconnect() {
    if (Connected) {
        Connected = false;
        if (L && CallbackReference != LUA_NOREF && CallbackReference != LUA_REFNIL) {
            lua_State *mainState = lua_mainthread(L);
            lua_unref(mainState, CallbackReference);
            CallbackReference = LUA_NOREF;
            L = nullptr;
        }
    }
}

int SignalConnection::LGarbageCollect(lua_State *L, SignalConnection *self) {
    if (self) {
        self->Disconnect();
    }
    return 0;
}

std::string_view BaseSignal::GetUserdataType() { return "Signal"; }
UserdataTag BaseSignal::GetUserdataTag() { return UserdataTag::Signal; }

const BaseSignal::UserdataProperties &BaseSignal::GetUserdataProperties() {
    static const BaseSignal::UserdataProperties PROPERTIES{};
    return PROPERTIES;
}

const BaseSignal::UserdataMethods &BaseSignal::GetUserdataMethods() {
    static const BaseSignal::UserdataMethods METHODS{
        {"Connect", {BaseSignal::LConnect}},
        {"Once", {BaseSignal::LOnce}},
        {"Wait", {BaseSignal::LWait}},
    };
    return METHODS;
}

SignalConnection::Pointer
BaseSignal::Connect(std::function<void(std::any)> callback, lua_State *L, int callbackReference) {
    auto connection = std::make_shared<SignalConnection>(callback, L, callbackReference);
    Connections.push_back(connection);
    return connection;
};

SignalConnection::Pointer
BaseSignal::Once(std::function<void(std::any)> callback, lua_State *L, int callbackReference) {
    auto connection = std::make_shared<SignalConnection>(nullptr, L, callbackReference);
    std::weak_ptr<SignalConnection> weakConnection = connection;
    connection->Callback = [weakConnection, callback](CallbackArgument value) {
        if (auto conn = weakConnection.lock()) {
            conn->Disconnect();
        }

        if (callback) {
            callback(value);
        }
    };
    Connections.push_back(connection);
    return connection;
};

void BaseSignal::Fire(CallbackArgument value) {
    for (auto it = Connections.begin(); it != Connections.end();) {
        auto &connection = *it;
        if (!connection || !connection->Connected) {
            it = Connections.erase(it);
        } else {
            if (connection->Callback) {
                connection->Callback(value);
            }
            ++it;
        }
    }
}

int BaseSignal::LConnect(lua_State *L, BaseSignal *signal) {
    DumpLuaStack(L);
    int callbackReference = LReferenceCallback(L, 2);
    DumpLuaStack(L);
    return StackValue<SignalConnection::Pointer>::Push(
        L, signal->Connect(
               [L, callbackReference, signal](CallbackArgument value) {
                   LRunCallback(L, signal, callbackReference, value);
               },
               L, callbackReference
           )
    );
}

int BaseSignal::LOnce(lua_State *L, BaseSignal *signal) {
    int callbackReference = LReferenceCallback(L, 2);
    return StackValue<SignalConnection::Pointer>::Push(
        L, signal->Once(
               [L, callbackReference, signal](CallbackArgument value) {
                   LRunCallback(L, signal, callbackReference, value);
               },
               L, callbackReference
           )
    );
}

int BaseSignal::LWait(lua_State *L, BaseSignal *signal) {
    signal->Once(
        [L, signal](CallbackArgument value) {
            int argumentCount = signal->LPushArgument(L, value);
            int status = lua_resume(L, nullptr, argumentCount);
            if (status != LUA_OK && status != LUA_YIELD) {
                SDL_Log("Failed to resume thread after signal: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
            };
        },
        L, LUA_NOREF
    );
    return lua_yield(L, 0);
}

int BaseSignal::LReferenceCallback(lua_State *L, int idx) {
    if (!lua_isfunction(L, idx)) {
        luaL_typeerror(L, idx, "function");
    }

    lua_pushvalue(L, idx);
    int ref = lua_ref(L, -1);
    lua_pop(L, 1);

    return ref;
}

void BaseSignal::LRunCallback(lua_State *L, BaseSignal *signal, int callbackReference, std::any value) {
    if (callbackReference == LUA_NOREF || callbackReference == LUA_REFNIL) {
        return;
    }

    lua_State *mainState = lua_mainthread(L);
    lua_getref(mainState, callbackReference);

    if (!lua_isfunction(mainState, -1)) {
        lua_pop(mainState, 1);
        return;
    }

    int arguments = signal->LPushArgument(mainState, value);
    if (lua_pcall(mainState, arguments, 0, 0) != LUA_OK) {
        SDL_Log("Signal error: %s", lua_tostring(mainState, -1));
        lua_pop(mainState, 1);
    }
}

} // namespace gargantuan
