#include "gargantuan/classes/list/DataModel.hpp"
#include "gargantuan/datatypes/Instance.hpp"

namespace gargantuan {

const Instance::ClassDefinition DataModel::DEFINITION = {
    .Name = "DataModel",
    .Superclass = "Instance",
};

}
