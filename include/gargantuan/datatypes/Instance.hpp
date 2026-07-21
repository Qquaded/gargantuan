#pragma once

#include "gargantuan/scripting/StackValue.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <functional>
#include <lua.h>
#include <memory>
#include <string_view>
#include <vector>

namespace gargantuan {

class Instance : public std::enable_shared_from_this<Instance>, public Userdata<Instance, std::shared_ptr<Instance>> {
  public:
    typedef std::shared_ptr<Instance> Pointer;
    typedef Userdata<Instance, std::shared_ptr<Instance>> InstanceUserdata;
    struct ClassDefinition {
        std::string_view Name;
        std::optional<std::string_view> Superclass;

        std::function<Pointer()> Constructor;
        template <typename T> static std::function<std::shared_ptr<Instance>()> WrapConstructor() {
            return []() -> std::shared_ptr<Instance> { return std::make_shared<T>(); };
        }

        std::unordered_map<std::string_view, InstanceUserdata::Property> Properties = {};
        std::unordered_map<std::string_view, InstanceUserdata::Method> Methods = {};
    };

    static const ClassDefinition DEFINITION;

    virtual ~Instance() = default;

    std::string_view Name = DEFINITION.Name;
    std::vector<std::shared_ptr<Instance>> Children;
    Instance *Parent = nullptr;
    void SetParent(std::shared_ptr<Instance> newParent);

    template <typename T> bool IsA() const { return dynamic_cast<const T *>(this) != nullptr; }
    template <typename T> const T *Cast() const { return dynamic_cast<const T *>(this); }

    std::optional<InstanceUserdata::Property> FindProperty(std::string_view name);
    std::optional<InstanceUserdata::Method> FindMethod(std::string_view name);
    static int UserdataIndex(lua_State *L);
    static int UserdataNewIndex(lua_State *L);
    static int UserdataNamecall(lua_State *L);

    std::string GetFullName();
    std::vector<std::shared_ptr<Instance>> GetDescendants();
    std::shared_ptr<Instance> FindFirstChild(std::string_view name, bool recursive = false);
    std::shared_ptr<Instance> FindFirstChildOfClass(std::string_view className);
    std::shared_ptr<Instance> FindFirstChildWhichIsA(std::string_view className);
    std::shared_ptr<Instance> FindFirstDescendant(std::string_view name);
    std::shared_ptr<Instance> FindFirstDescendantOfClass(std::string_view className);
    std::shared_ptr<Instance> FindFirstDescendantWhichIsA(std::string_view className);

  private:
    void CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants);
};

template <> struct StackValue<Instance::Pointer> {
    static inline std::string_view ReflectedTypedef() { return StackValue<Instance::Userdata>::ReflectedTypedef(); };
    static bool Is(lua_State *L, int idx) { return StackValue<Instance::Userdata>::Is(L, idx); };
    static Instance::Pointer From(lua_State *L, int idx) { return StackValue<Instance::Userdata>::From(L, idx); };
    static void Push(lua_State *L, Instance::Pointer value) { return StackValue<Instance::Userdata>::Push(L, value); };
};

} // namespace gargantuan
