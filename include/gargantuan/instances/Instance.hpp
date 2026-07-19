#pragma once

#include "gargantuan/instances/ClassDefinition.hpp"

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
    std::vector<std::shared_ptr<Instance>> GetDescendants();

    template <typename T> bool IsA() const { return dynamic_cast<const T *>(this) != nullptr; }
    template <typename T> const T *Cast() const { return dynamic_cast<const T *>(this); }

  private:
    void CollectDescendants(std::vector<std::shared_ptr<Instance>> &descendants);
};

} // namespace gargantuan::instances
