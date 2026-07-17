#pragma once

#include <memory>
#include <string>
#include <vector>

namespace gargantuan::datatypes {

class Instance {
  public:
    std::string Name;
    std::string ClassName;

    Instance *Parent = nullptr;
    std::vector<std::shared_ptr<Instance>> Children;

    Instance(std::string className) : Name(className), ClassName(className) {}
    virtual ~Instance() = default;

    void SetParent(Instance *newParent);
};

} // namespace gargantuan::datatypes
