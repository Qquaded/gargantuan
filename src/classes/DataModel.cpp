#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/services/RunService.hpp"
#include "gargantuan/services/TweenService.hpp"
#include "gargantuan/services/UserInputService.hpp"
#include "gargantuan/services/Workspace.hpp"

namespace gargantuan {
	G_INSTANCE_IMPL(DataModel, .Superclass = "ServiceProvider");

	const DataModel::ServiceConstructors &DataModel::GetServiceConstructors() const {
		static const DataModel::ServiceConstructors CONSTRUCTORS = {
			{"Workspace", Workspace::CLASS_DEFINITION.Constructor},
			{"UserInputService", UserInputService::CLASS_DEFINITION.Constructor},
			{"RunService", RunService::CLASS_DEFINITION.Constructor},
			{"TweenService", TweenService::CLASS_DEFINITION.Constructor},
		};
		return CONSTRUCTORS;
	};
} // namespace gargantuan
