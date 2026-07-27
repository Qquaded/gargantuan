#include "gargantuan/classes/DataModel.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/services/RunService.hpp"
#include "gargantuan/services/Workspace.hpp"
#include "gargantuan/services/TweenService.hpp"

namespace gargantuan {
	const DataModel::ClassDefinition DataModel::DEFINITION = {
		.Name = "DataModel",
		.Superclass = "ServiceProvider",
	};

	const DataModel::ServiceConstructors &DataModel::GetServiceConstructors() const {
		static const DataModel::ServiceConstructors CONSTRUCTORS = {
			{"Workspace", Workspace::DEFINITION.Constructor},
			{"RunService", RunService::DEFINITION.Constructor},
			{"TweenService", TweenService::DEFINITION.Constructor}
		};
		return CONSTRUCTORS;
	};
} // namespace gargantuan
