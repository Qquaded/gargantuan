#pragma once

#include "gargantuan/instances/BasePart.hpp"

namespace gargantuan::instances {

enum class Shape { Block };

class Part : public BasePart {
  public:
    Shape Shape = Shape::Block;

    render::Mesh GetMesh() override;
};

} // namespace gargantuan::instances
