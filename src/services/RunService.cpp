#include "gargantuan/services/RunService.hpp"
#include "gargantuan/datatypes/Signal.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	const RunService::ClassDefinition RunService::DEFINITION = {
		.Name = "RunService",
		.Superclass = "Instance",
		.Constructor = ClassDefinition::WrapConstructor<RunService>(),
		.Properties = {
			G_UD_READONLY_PROP(RunService, PreRender, Signal<double>::Pointer),
		}
	};
}
