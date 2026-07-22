#pragma once

#include "gargantuan/classes/list/BasePart.hpp"
#include "gargantuan/datatypes/Instance.hpp"

namespace gargantuan {

namespace Enums {
enum class PartType {
    Ball,
    Block,
    Cylinder,
    Wedge,
    CornerWedge,
};
};

class Part : public BasePart {
  public:
    static const ClassDefinition DEFINITION;

    Enums::PartType Shape = Enums::PartType::Block;
    std::unique_ptr<GpuMesh> &GetMesh(MeshProvider &provider) const override;
};

} // namespace gargantuan
