#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"
#include <SDL3/SDL_log.h>
#include <lua.h>
#include <memory>

namespace gargantuan {

std::string_view SignalConnection::GetUserdataType() { return "SignalConnection"; }
UserdataTag SignalConnection::GetUserdataTag() { return UserdataTag::SignalConnection; }

SignalConnection::SignalConnection(SignalConnection::CallbackType callback) : Callback(callback) {};

const SignalConnection::UserdataProperties &SignalConnection::GetUserdataProperties() {
    static const SignalConnection::UserdataProperties PROPERTIES{
        USERDATA_READONLY_PROP(SignalConnection, Connected, bool),
    };
    return PROPERTIES;
}

const SignalConnection::UserdataMethods &SignalConnection::GetUserdataMethods() {
    static const SignalConnection::UserdataMethods METHODS{
        {"Disconnect", Method::Wrap<&SignalConnection::Disconnect>()},
        {"Reconnect", Method::Wrap<&SignalConnection::Reconnect>()},
    };
    return METHODS;
}

void SignalConnection::Disconnect() { Connected = false; }
void SignalConnection::Reconnect() { Connected = true; }

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

SignalConnection::Pointer BaseSignal::Connect(std::function<void(std::any)> callback) {
    auto connection = std::make_shared<SignalConnection>(callback);
    Connections.push_back(connection);
    return connection;
};

SignalConnection::Pointer BaseSignal::Once(std::function<void(std::any)> callback) {
    auto connection = std::make_shared<SignalConnection>(nullptr);
    connection->Callback = [connection, callback](CallbackArgument value) {
        connection->Disconnect();
        callback(value);
    };
    Connections.push_back(connection);
    return connection;
};

void BaseSignal::Fire(CallbackArgument value) {
    for (auto &connection : Connections) {
        if (connection->Connected && connection->Callback) {
            connection->Callback(value);
        }
    }
}

int BaseSignal::LConnect(lua_State *L, BaseSignal *signal) {
    int callbackReference = LReferenceCallback(L, 2);
    return StackValue<SignalConnection::Pointer>::Push(
        L, signal->Connect([L, callbackReference, signal](CallbackArgument value) {
            LRunCallback(L, signal, callbackReference, value);
        })
    );
}

int BaseSignal::LOnce(lua_State *L, BaseSignal *signal) {
    int callbackReference = LReferenceCallback(L, 2);
    return StackValue<SignalConnection::Pointer>::Push(
        L, signal->Once([L, callbackReference, signal](CallbackArgument value) {
            LRunCallback(L, signal, callbackReference, value);
        })
    );
}

int BaseSignal::LWait(lua_State *L, BaseSignal *signal) {
    signal->Once([L, signal](CallbackArgument value) {
        int argumentCount = signal->LPushArgument(L, value);
        int status = lua_resume(lua_mainthread(L), L, argumentCount);
        if (status != LUA_OK && status != LUA_YIELD) {
            SDL_Log("Failed to resume thread after signal: %s", lua_tostring(L, -1));
            lua_pop(L, 1);
        };
    });
    return lua_yield(L, 0);
}

int BaseSignal::LReferenceCallback(lua_State *L, int idx) {
    if (!lua_isfunction(L, idx)) {
        luaL_typeerror(L, idx, "function");
    }
    // lua_ref keeps the value on the stack, no need to pushvalue/pop
    return lua_ref(L, idx);
}

void BaseSignal::LRunCallback(lua_State *L, BaseSignal *signal, int callbackReference, std::any value) {
    lua_getref(L, callbackReference);
    int arguments = signal->LPushArgument(L, value);
    if (lua_pcall(L, arguments, 0, 0) != LUA_OK) {
        SDL_Log("Signal error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    };
}

} // namespace gargantuan
