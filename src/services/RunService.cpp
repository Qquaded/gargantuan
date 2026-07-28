#include "gargantuan/services/RunService.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	const RunService::ClassDefinition RunService::DEFINITION = {
		.Name = "RunService",
		.Superclass = "Instance",
		.Constructor = ClassDefinition::WrapConstructor<RunService>(),
		.Properties = {
			{"PreRender", Property::fromSimple<&RunService::PreRender>(true, false)},
		}
	};
}
