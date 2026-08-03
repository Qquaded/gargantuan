#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/classes/Part.hpp"
#include "gargantuan/physics/Conversions.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL_log.h>
#include <box3d/box3d.h>
#include <box3d/collision.h>
#include <box3d/types.h>
#include <common.hpp>
#include <memory>
#include <unordered_map>

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(WorldRoot);

	WorldRoot::WorldRoot() {
		b3WorldDef worldDef = b3DefaultWorldDef();
		// put stuff to change the worlddef (probably based on project.config.json)
		// stuff like gravity and etc
		worldDef.gravity = b3Vec3{0.0f, -10.0f, 0.0f};
		b3WorldId worldId = b3CreateWorld(&worldDef);
		World = worldId;

		auto checkChildAdded = [this](Instance::Pointer instance) {
			if (instance->IsA("BasePart")) {
				std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
				this->Parts.push_back(part);

				b3BodyDef partBodyDef = b3DefaultBodyDef();
				partBodyDef.position = ToBox3(part->CFrame.Position);
				partBodyDef.rotation = ToBox3(part->CFrame.ToQuaternion());
				b3ShapeDef partShapeDef = b3DefaultShapeDef();
				if (part->Anchored == true) {
					partBodyDef.type = (b3BodyType)b3_staticBody;
				} else {
					partBodyDef.type = (b3BodyType)b3_dynamicBody;
					partShapeDef.density = 0.7f;
				};

				if (part->CanCollide == false) {
					partShapeDef.isSensor = true;
					partShapeDef.enableSensorEvents = true; // CanTouch
				}

				partBodyDef.userData = part.get();
				b3BodyId partId = b3CreateBody(World, &partBodyDef);
				// hull
				if (const Part *partNotBasePart = part->Cast<Part>()) {
					switch (partNotBasePart->Shape) {
					case Enums::PartType::Block:
					case Enums::PartType::Wedge:
					case Enums::PartType::CornerWedge: {
						b3BoxHull partBox = b3MakeBoxHull(
							part->Size.x * 0.5f, part->Size.y * 0.5f, part->Size.z * 0.5f
						);
						b3CreateHullShape(partId, &partShapeDef, &partBox.base);
						break;
					}
					case Enums::PartType::Ball: {
						b3Sphere partSphere = b3Sphere{
							.center = {0, 0, 0}, .radius = fmin(fmin(part->Size.x, part->Size.y), part->Size.z) * 0.5f
						};
						b3CreateSphereShape(partId, &partShapeDef, &partSphere);
						break;
					}
					case Enums::PartType::Cylinder: {
						b3HullData *cylinderHullData = b3CreateCylinder(
							part->Size.y, fmin(part->Size.x * 0.5f, part->Size.z * 0.5f), 0, 20
						); // idk 20 sides seems fine
						b3CreateHullShape(partId, &partShapeDef, cylinderHullData);
						b3DestroyHull(cylinderHullData);
						break;
					}
					};
				} else {
					b3BoxHull partBox = b3MakeBoxHull(part->Size.x * 0.5f, part->Size.y * 0.5f, part->Size.z * 0.5f);
					b3CreateHullShape(partId, &partShapeDef, &partBox.base);
				}

				this->PartBodies[part.get()] = partId;
			}
		};

		auto checkChildRemoved = [this](Instance::Pointer instance) {
			if (instance->IsA("BasePart")) {
				std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
				erase(Parts, part);
				b3DestroyBody(this->PartBodies[part.get()]);
				this->PartBodies.erase(part.get());
			}
		};

		DescendantAdded->Connect(checkChildAdded);
		DescendantRemoved->Connect(checkChildRemoved);
	};

	void WorldRoot::KillWorld() {
		b3DestroyWorld(World);
	}
} // namespace gargantuan
