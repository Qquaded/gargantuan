#include "gargantuan/services/RunService.hpp"

namespace gargantuan {

const RunService::ClassDefinition RunService::DEFINITION = {
    .Name = "RunService",
    .Constructor = ClassDefinition::WrapConstructor<RunService>(),
};

}
