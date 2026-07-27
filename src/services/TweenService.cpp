#include "gargantuan/services/TweenService.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	const TweenService::ClassDefinition TweenService::DEFINITION = {
		.Name = "TweenService",
		.Superclass = "Instance",
		.Constructor = ClassDefinition::WrapConstructor<TweenService>(),
		.Properties = {
			
		}
	};
}
