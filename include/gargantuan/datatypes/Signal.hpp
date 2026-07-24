#pragma once

#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

#include <SDL3/SDL_log.h>
#include <any>
#include <functional>
#include <lua.h>
#include <lualib.h>
#include <memory>
#include <string_view>
#include <vector>

namespace gargantuan {

enum class SignalSource {
    Engine,
    User,
};

struct SignalConnection : public Userdata<SignalConnection, std::shared_ptr<SignalConnection>>,
                          public std::enable_shared_from_this<SignalConnection> {
  public:
    typedef std::shared_ptr<SignalConnection> Pointer;
    typedef Userdata<SignalConnection, std::shared_ptr<SignalConnection>> This;

    typedef std::any CallbackArgument;
    typedef std::function<void(CallbackArgument)> CallbackType;

    SignalConnection(CallbackType callback);

    CallbackType Callback;
    bool Connected = true;
    void Disconnect();
    void Reconnect();

    static std::string_view GetUserdataType();
    static UserdataTag GetUserdataTag();
    static const SignalConnection::UserdataProperties &GetUserdataProperties();
    static const SignalConnection::UserdataMethods &GetUserdataMethods();
};

// NOTE: Split the Luau implementation into BaseSignal a class to avoid
// needing a template, which makes StackValue implementations trivial.
// TODO: Unref the connection callbacks somewhere
struct BaseSignal : public Userdata<BaseSignal, std::shared_ptr<BaseSignal>>,
                    public std::enable_shared_from_this<BaseSignal> {
  public:
    typedef std::shared_ptr<BaseSignal> Pointer;
    typedef Userdata<BaseSignal, Pointer> This;

    typedef std::any CallbackArgument;
    typedef std::function<void(CallbackArgument)> CallbackType;

    static std::string_view GetUserdataType();
    static UserdataTag GetUserdataTag();
    static const BaseSignal::UserdataProperties &GetUserdataProperties();
    static const BaseSignal::UserdataMethods &GetUserdataMethods();

    SignalSource Source = SignalSource::Engine;
    std::vector<SignalConnection::Pointer> Connections;

  protected:
    SignalConnection::Pointer Connect(CallbackType callback);
    SignalConnection::Pointer Once(CallbackType callback);
    void Fire(CallbackArgument argument);

    virtual int LPushArgument(lua_State *L, CallbackArgument value) = 0;

    static int LConnect(lua_State *L, BaseSignal *signal);
    static int LOnce(lua_State *L, BaseSignal *signal);
    static int LWait(lua_State *L, BaseSignal *signal);

    static int LReferenceCallback(lua_State *L, int idx);
    static void LRunCallback(lua_State *L, BaseSignal *signal, int callbackReference, std::any value);
};

template <typename T> struct Signal : BaseSignal {
    typedef std::shared_ptr<Signal> Pointer;
    typedef Userdata<Signal, Pointer> This;

    typedef T CallbackArgument;
    typedef std::function<void(T)> CallbackType;

    SignalConnection::Pointer Connect(CallbackType callback) {
        return BaseSignal::Connect([callback](std::any value) { callback(std::any_cast<T>(value)); });
    }

    SignalConnection::Pointer Once(CallbackType callback) {
        return BaseSignal::Once([callback](std::any value) { callback(std::any_cast<T>(value)); });
    }

    void Fire(T argument) { BaseSignal::Fire(std::any(argument)); }

    int LPushArgument(lua_State *L, std::any value) override {
        return StackValue<T>::Push(L, std::any_cast<T>(value));
    };

    static std::string_view GetUserdataType() { return BaseSignal::GetUserdataType(); };
    static UserdataTag GetUserdataTag() { return BaseSignal::GetUserdataTag(); };
    static const BaseSignal::UserdataProperties &GetUserdataProperties() {
        return BaseSignal::GetUserdataProperties();
    };
    static const BaseSignal::UserdataMethods &GetUserdataMethods() { return BaseSignal::GetUserdataMethods(); };
};

#define CLASS_SIGNAL(propertyName, signalType)                                                                         \
    Signal<signalType>::Pointer propertyName = std::make_shared<Signal<signalType>>();

USERDATA_STACKVALUE_WITH_STORED(SignalConnection, SignalConnection::Pointer)
USERDATA_STACKVALUE_WITH_STORED(BaseSignal, BaseSignal::Pointer)

template <typename T> struct StackValue<std::shared_ptr<Signal<T>>> {
    static inline std::string_view ReflectedTypedef() { return StackValue<BaseSignal::Pointer>::ReflectedTypedef(); };
    static bool Is(lua_State *L, int idx) { return StackValue<BaseSignal::Pointer>::Is(L, idx); };
    static std::shared_ptr<Signal<T>> From(lua_State *L, int idx) {
        return StackValue<BaseSignal::Pointer>::From(L, idx);
    };
    static int Push(lua_State *L, std::shared_ptr<Signal<T>> value) {
        return StackValue<BaseSignal::Pointer>::Push(L, value);
    };
};

} // namespace gargantuan
