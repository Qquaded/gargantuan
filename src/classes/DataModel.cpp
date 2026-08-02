#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/services/RunService.hpp"
#include "gargantuan/services/TweenService.hpp"
#include "gargantuan/services/UserInputService.hpp"
#include "gargantuan/services/Workspace.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(
		DataModel,
		.Description = "The root hierachy and service provider for Gargantuan games.",
		.Superclass = "ServiceProvider"
	);

	const DataModel::ServiceDefinitions &DataModel::GetServiceDefinitions() const {
		static const DataModel::ServiceDefinitions CONSTRUCTORS = {
			{"Workspace", Workspace::CLASS_DEFINITION},
			{"UserInputService", UserInputService::CLASS_DEFINITION},
			{"RunService", RunService::CLASS_DEFINITION},
			{"TweenService", TweenService::CLASS_DEFINITION},
		};
		return CONSTRUCTORS;
	};
}
