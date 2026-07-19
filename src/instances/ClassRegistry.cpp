#include "gargantuan/instances/ClassRegistry.hpp"
#include "gargantuan/instances/ClassDefinition.hpp"
#include "gargantuan/instances/Instance.hpp"
#include "gargantuan/instances/list/BasePart.hpp"
#include "gargantuan/instances/list/DataModel.hpp"
#include "gargantuan/instances/list/Part.hpp"

#include <vector>

namespace gargantuan::instances::ClassRegistry {
const std::vector<ClassDefinition> DEFINITIONS = {
    Instance::DEFINITION,
    BasePart::DEFINITION,
    DataModel::DEFINITION,
    Part::DEFINITION,
};
}
