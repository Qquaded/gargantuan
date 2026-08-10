#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/physics/Conversions.hpp"

#include <SDL3/SDL.h>

#include <box3d/box3d.h>
#include <box3d/collision.h>
#include <box3d/math_functions.h>
#include <box3d/types.h>
#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace gargantuan {
	WorldRoot::WorldRoot() {
		b3WorldDef worldDefinition = b3DefaultWorldDef();
		worldDefinition.enableSleep = true;
		worldDefinition.gravity = b3Vec3{0.0f, Gravity, 0.0f};

		World = b3CreateWorld(&worldDefinition);
		Destroying->Once([world = this->World](std::monostate _) { b3DestroyWorld(world); });

		BindDescendants([this](std::shared_ptr<Instance> instance) {
			auto part = std::dynamic_pointer_cast<BasePart>(instance);
			if (!part) return;

			this->Parts.push_back(part);

			b3BodyDef bodyDefinition = b3DefaultBodyDef();
			bodyDefinition.position = ToBox3(part->GetCFrame().Position);
			bodyDefinition.rotation = ToBox3(part->GetCFrame().ToQuaternion());

			b3ShapeDef shapeDefinition = b3DefaultShapeDef();

			// TODO: Maybe generate BindProperty methods + a Life class akin to
			// WTH for tracking signals from that
			if (part->GetAnchored()) {
				bodyDefinition.type = (b3BodyType)b3_staticBody;
			} else {
				bodyDefinition.type = (b3BodyType)b3_dynamicBody;
				shapeDefinition.density = 0.7f;
			};

			if (part->GetCanCollide()) {
				shapeDefinition.isSensor = false;
			} else {
				shapeDefinition.isSensor = true;
				shapeDefinition.enableSensorEvents = part->GetCanTouch();
			}

			bodyDefinition.userData = part.get();
			b3BodyId bodyId = b3CreateBody(World, &bodyDefinition);
			part->CreateBodyShape(bodyId, shapeDefinition);
			this->PartBodies[part.get()] = bodyId;
		});

		DescendantRemoved->Connect([this](std::shared_ptr<Instance> instance) {
			if (auto part = std::static_pointer_cast<BasePart>(instance)) {
				erase(Parts, part);
				b3DestroyBody(this->PartBodies[part.get()]);
				this->PartBodies.erase(part.get());
			}
		});
	};

	void WorldRoot::StepPhysics(double deltaTime, std::optional<std::vector<std::shared_ptr<Instance>>> instances) {
		StepAccumulator += deltaTime;

		int steps = 0;
		while (StepAccumulator >= STEP_INTERVAL && steps < MAX_STEPS_PER_FRAME) {
			b3World_Step(World, STEP_INTERVAL, SUB_STEP_COUNT);

			b3BodyEvents events = b3World_GetBodyEvents(World);
			for (int i = 0; i < events.moveCount; ++i) {
				const b3BodyMoveEvent &move = events.moveEvents[i];

				BasePart *part = static_cast<BasePart *>(move.userData);
				if (part == nullptr) continue;
				part->SetCFrame(
					gargantuan::CFrame(FromBox3(move.transform.p), glm::mat3_cast(FromBox3(move.transform.q)))
				);
			}
			StepAccumulator -= STEP_INTERVAL;
			++steps;
		}

		if (steps == MAX_STEPS_PER_FRAME) StepAccumulator = 0.0f;
	}

}
