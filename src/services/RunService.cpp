#include "gargantuan/services/RunService.hpp"

namespace gargantuan {

const RunService::ClassDefinition RunService::DEFINITION = {
    .Name = "RunService",
    .Superclass = "Instance",
    .Constructor = ClassDefinition::WrapConstructor<RunService>(),
};

}
