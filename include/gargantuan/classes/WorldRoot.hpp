#pragma once

#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/generated/WorldRoot.hpp"

#include <box3d/box3d.h>
#include <box3d/id.h>
#include <memory>
#include <vector>

namespace gargantuan {
	class WorldRoot : public Instance {
		I_WorldRoot;

		WorldRoot();

		static constexpr int MAX_STEPS_PER_FRAME = 4;
		static constexpr float STEP_INTERVAL = 1.0f / 60.0f;
		static constexpr int SUB_STEP_COUNT = 4;
		float StepAccumulator = 0.0f;

		b3WorldId World;

		std::vector<std::shared_ptr<BasePart>> Parts;
		std::unordered_map<BasePart *, b3BodyId> PartBodies;

		void StepPhysics(float deltaTime);
	};
}
