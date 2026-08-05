#include "gargantuan/services/RunService.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(
		RunService,
		.Properties = {
			{"PreRender", Property::fromMember<&RunService::PreRender>(true, false)},
			{"PreSimulation", Property::fromMember<&RunService::PreSimulation>(true, false)},
			{"PostSimulation", Property::fromMember<&RunService::PostSimulation>(true, false)},
		}
	);
}
