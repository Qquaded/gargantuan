#pragma once

#include "gargantuan/scripting/ScriptEngine.hpp"
#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <SDL3/SDL_log.h>
#include <functional>
#include <lua.h>
#include <lualib.h>
#include <memory>
#include <vector>

namespace gargantuan {

#define CLASS_SIGNAL(propertyName, signalType)                                                                         \
    Signal<signalType>::Pointer propertyName = std::make_shared<Signal<signalType>>();

// TODO: implement lua side disconnect and unref the callback
template <typename T>
struct SignalConnection : public Userdata<SignalConnection<T>, std::shared_ptr<SignalConnection<T>>> {
    typedef Userdata<SignalConnection<T>, std::shared_ptr<SignalConnection<T>>> This;
    typedef std::shared_ptr<SignalConnection<T>> Pointer;

    SignalConnection() = default;
    SignalConnection(std::function<void(T)> callback) : Callback(std::move(callback)) {}

    std::function<void(T)> Callback;
    bool Connected = true;
    void Disconnect() { Connected = false; }
    void Reconnect() { Connected = true; }

    static UserdataTag GetUserdataTag() { return UserdataTag::SignalConnection; }
    static std::string_view GetUserdataType() { return "SignalConnection"; }

    static const This::UserdataProperties &GetUserdataProperties() {
        static const typename This::UserdataProperties PROPERTIES = {};
        return PROPERTIES;
    }

    static const This::UserdataMethods &GetUserdataMethods() {
        static const typename This::UserdataMethods METHODS = {};
        return METHODS;
    }
};

template <typename T> class Signal : public Userdata<Signal<T>, std::shared_ptr<Signal<T>>> {
  public:
    typedef Userdata<Signal<T>, std::shared_ptr<Signal<T>>> This;
    typedef std::shared_ptr<Signal<T>> Pointer;
    typedef std::function<void(T)> SignalCallback;

    std::vector<std::shared_ptr<SignalConnection<T>>> Connections = {};

    Signal() {}

    std::shared_ptr<SignalConnection<T>> Connect(SignalCallback callback) {
        std::shared_ptr<SignalConnection<T>> connection = std::make_shared<SignalConnection<T>>(callback);
        Connections.push_back(connection);
        return connection;
    };

    std::shared_ptr<SignalConnection<T>> Once(SignalCallback callback) {
        auto connection = std::make_shared<std::shared_ptr<SignalConnection<T>>>();
        *connection = Connect([callback, connection](T value) {
            if (*connection) {
                (*connection)->Disconnect();
            }
            callback(value);
        });
        return *connection;
    };

    void Fire(T value) {
        for (const auto &connection : Connections) {
            if (connection && connection->Connected) {
                connection->Callback(value);
            }
        }
    }

    static UserdataTag GetUserdataTag() { return UserdataTag::Signal; }
    static std::string_view GetUserdataType() { return "Signal"; }

    static const This::UserdataProperties &GetUserdataProperties() {
        static const typename This::UserdataProperties PROPERTIES = {};
        return PROPERTIES;
    }

    static const This::UserdataMethods &GetUserdataMethods() {
        static const typename This::UserdataMethods METHODS = {
            {"Connect", {[](lua_State *L, Signal<T> *instance) -> int {
                 int callbackReference = TakeCallbackReference(L, 2);
                 return StackValue<std::shared_ptr<SignalConnection<T>>>::Push(
                     L, instance->Connect([L, callbackReference](T value) {
                         RunCallbackReference(L, callbackReference, value);
                     })
                 );
             }}},

            {"Once", {[](lua_State *L, Signal<T> *instance) -> int {
                 int callbackReference = TakeCallbackReference(L, 2);
                 return StackValue<std::shared_ptr<SignalConnection<T>>>::Push(
                     L, instance->Once([L, callbackReference](T value) {
                         RunCallbackReference(L, callbackReference, value);
                     })
                 );
             }}},

            {"Wait", {[](lua_State *L, Signal<T> *instance) -> int {
                 instance->Once([L](T value) {
                     int argumentCount = StackValue<T>::Push(L, value);
                     int status = lua_resume(lua_mainthread(L), L, argumentCount);
                     if (status != LUA_OK && status != LUA_YIELD) {
                         SDL_Log("Failed to resume thread after signal: %s", lua_tostring(L, -1));
                         lua_pop(L, 1);
                     };
                 });
                 return lua_yield(L, 0);
             }}},
        };
        return METHODS;
    }

  private:
    static int TakeCallbackReference(lua_State *L, int idx) {
        if (!lua_isfunction(L, idx)) {
            luaL_typeerror(L, idx, "function");
        }
        // lua_ref keeps the value on the stack, no need to pushvalue/pop
        return lua_ref(L, idx);
    }

    static void RunCallbackReference(lua_State *L, int callbackReference, T value) {
        lua_getref(L, callbackReference);
        int arguments = StackValue<T>::Push(L, value);
        DumpLuaStack(L);
        if (lua_pcall(L, arguments, 0, 0) != LUA_OK) {
            SDL_Log("Signal error: %s", lua_tostring(L, -1));
            lua_pop(L, 1);
        };
    }
};

// TODO: make macros cuz this is abhorrent

template <typename T> struct StackValue<std::shared_ptr<Signal<T>>> {
    typedef Signal<T> This;
    typedef std::shared_ptr<Signal<T>> ThisPointer;

    static inline std::string_view ReflectedTypedef() { return This::GetUserdataType(); }
    static bool Is(lua_State *L, int idx) { return lua_userdatatag(L, idx) == (int)This::GetUserdataTag(); }

    static ThisPointer From(lua_State *L, int idx) {
        auto *userdata =
            static_cast<std::shared_ptr<This> *>(lua_touserdatatagged(L, idx, (int)This::GetUserdataTag()));
        return *userdata;
    }

    static int Push(lua_State *L, ThisPointer value) {
        void *storage = lua_newuserdatataggedwithmetatable(L, sizeof(ThisPointer), (int)This::GetUserdataTag());
        new (storage) ThisPointer(value);
        return 1;
    }
};

template <typename T> struct StackValue<Signal<T>> {
    typedef Signal<T> This;
    typedef std::shared_ptr<Signal<T>> ThisPointer;

    static inline std::string_view ReflectedTypedef() { return This::GetUserdataType(); }
    static bool Is(lua_State *L, int idx) { return lua_userdatatag(L, idx) == (int)This::GetUserdataTag(); }

    static This &From(lua_State *L, int idx) {
        auto ptr = StackValue<ThisPointer>::From(L, idx);
        return *ptr;
    }

    static int Push(lua_State *L, const This &value) {
        return StackValue<ThisPointer>::Push(L, std::make_shared<This>(value));
    }
};

template <typename T> struct StackValue<std::shared_ptr<SignalConnection<T>>> {
    typedef SignalConnection<T> This;
    typedef std::shared_ptr<SignalConnection<T>> ThisPointer;

    static inline std::string_view ReflectedTypedef() { return This::GetUserdataType(); }
    static bool Is(lua_State *L, int idx) { return lua_userdatatag(L, idx) == (int)This::GetUserdataTag(); }

    static ThisPointer From(lua_State *L, int idx) {
        auto *userdata =
            static_cast<std::shared_ptr<This> *>(lua_touserdatatagged(L, idx, (int)This::GetUserdataTag()));
        return *userdata;
    }

    static int Push(lua_State *L, ThisPointer value) {
        void *storage = lua_newuserdatataggedwithmetatable(L, sizeof(ThisPointer), (int)This::GetUserdataTag());
        new (storage) ThisPointer(value);
        return 1;
    }
};

template <typename T> struct StackValue<SignalConnection<T>> {
    typedef SignalConnection<T> This;

    static inline std::string_view ReflectedTypedef() { return This::GetUserdataType(); };

    static bool Is(lua_State *L, int idx) { return lua_userdatatag(L, idx) == (int)This::GetUserdataTag(); };

    static This From(lua_State *L, int idx) {
        This *userdata = static_cast<This *>(lua_touserdatatagged(L, idx, (int)This::GetUserdataTag()));
        return *userdata;
    };

    static int Push(lua_State *L, This value) {
        This *userdata =
            static_cast<This *>(lua_newuserdatataggedwithmetatable(L, sizeof(This), (int)This::GetUserdataTag()));
        new (userdata) This(value);
        return 1;
    };
};

} // namespace gargantuan
