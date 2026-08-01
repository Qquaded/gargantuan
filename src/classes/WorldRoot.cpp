#include "gargantuan/classes/WorldRoot.hpp"
#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

#include <SDL3/SDL_log.h>
#include <memory>
#include <box3d/box3d.h>

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(WorldRoot);

	WorldRoot::WorldRoot() {
	    ///b3WorldDef worlddef = b3DefaultWorldDef();
		//put stuff to change the worlddef (probably based on project.config.json)
		//stuff like gravity and etc
		///worlddef.gravity = (b3Vec3){0.0f, -196.2f, 0.0f}
		///worlddef.
		///b3WorldId worldid = b3CreateWorld(&worldDef);

		auto checkChildAdded = [this](Instance::Pointer instance) {
			if (instance->IsA("BasePart")) {
				std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
				this->Parts.push_back(part);
				//create the physics (the fun part :D)
			}
		};

		auto checkChildRemoved = [this](Instance::Pointer instance) {
		    if (instance->IsA("BasePart")) {
    			std::shared_ptr<BasePart> part = std::static_pointer_cast<BasePart>(instance);
                std::erase(Parts, part); //add unique id so parts that are deleted won't delete eveyr part with the same values
			}
		};

		for (const auto &instance : GetDescendants()) { //isnt this useless? isn't there nothing in descendants yet? could be wrong
			checkChildAdded(instance);
		}
		DescendantAdded->Connect(checkChildAdded);
		DescendantRemoved->Connect(checkChildRemoved);
	}
} // namespace gargantuan
