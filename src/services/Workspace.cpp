#include "gargantuan/services/Workspace.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(
		Workspace,
		.Superclass = "WorldRoot",
		.Properties = {
			{"CurrentCamera", Property::fromMember<&Workspace::CurrentCamera>(true, false)},
		}
	);
}
