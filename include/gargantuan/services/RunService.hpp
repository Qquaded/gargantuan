#pragma once

#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/datatypes/Signal.hpp"

namespace gargantuan {

	class RunService : public Instance {
	  public:
		static const ClassDefinition DEFINITION;

		CLASS_SIGNAL(PreSimulation, double);
		CLASS_SIGNAL(PostSimulation, double);
		CLASS_SIGNAL(PreRender, double);
	};

} // namespace gargantuan
