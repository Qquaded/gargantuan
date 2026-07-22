#include "gargantuan/classes/list/Part.hpp"
#include "gargantuan/datatypes/Instance.hpp"

#include <SDL3/SDL_log.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <stdexcept>

namespace gargantuan {

const Instance::ClassDefinition Part::DEFINITION = {
    .Name = "Part",
    .Superclass = "BasePart",
    .Constructor = ClassDefinition::WrapConstructor<Part>(),
};

std::unique_ptr<GpuMesh> &Part::GetMesh(MeshProvider &provider) const {
    std::string key = "gargantuan://meshes/" + std::string(magic_enum::enum_name(Shape));
    auto it = provider.GpuMeshes.find(key);
    if (it == provider.GpuMeshes.end()) {
        throw std::runtime_error("??? no mesh");
    }
    return it->second;
};

} // namespace gargantuan
