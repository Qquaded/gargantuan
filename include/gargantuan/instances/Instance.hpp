#pragma once

#include <memory>
#include <string>
#include <vector>

namespace gargantuan::instances {

class Instance : public std::enable_shared_from_this<Instance> {
  public:
    const std::string CLASS_NAME = "Instance";

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
