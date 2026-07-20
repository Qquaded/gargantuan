#pragma once

#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/scripting/UserdataTags.hpp"

#include <lua.h>
#include <memory>
#include <string_view>
#include <vector>

namespace gargantuan::instances {

class Instance : public std::enable_shared_from_this<Instance> {
  public:
    static const ClassDefinition DEFINITION;

    std::string_view Name = "Instance";

    virtual ~Instance() = default;

    Instance *Parent = nullptr;
    std::vector<std::shared_ptr<Instance>> Children;

    void SetParent(std::shared_ptr<Instance> newParent);

    template <typename T> bool IsA() const { return dynamic_cast<const T *>(this) != nullptr; }
    template <typename T> const T *Cast() const { return dynamic_cast<const T *>(this); }

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

} // namespace gargantuan::instances

namespace gargantuan::scripting {

static const ScriptType<instances::Instance *> TYPE_INSTANCE = {
    .Name = "Instance",
    .LuauTypeAlias = "Instance",
    .IsStackValue = [](lua_State *L, int idx) -> bool {
        return lua_userdatatag(L, idx) == (int)UserdataTags::Instance;
    },
    .FromStackValue = [](lua_State *L, int idx) -> instances::Instance * {
        auto **userdata = static_cast<instances::Instance **>(lua_touserdata(L, idx));
        if (!userdata || !*userdata) {
            return nullptr;
        };

        instances::Instance *instance = *userdata;
        return instance;
    },
    .PushStackValue = [](lua_State *L, instances::Instance *value) -> void {
        if (!value) {
            lua_pushnil(L);
            return;
        };

        auto **userdata = static_cast<instances::Instance **>(
            lua_newuserdatataggedwithmetatable(L, sizeof(instances::Instance *), (int)UserdataTags::Instance)
        );

        *userdata = value;
    },
};

} // namespace gargantuan::scripting
