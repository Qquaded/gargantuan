#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/datatypes/Instance.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	const BasePart::ClassDefinition BasePart::DEFINITION = {
		.Name = "BasePart",
		.Superclass = "Instance",
		.Properties = {
			{"Anchored", Property::fromSimple<&BasePart::Anchored>(true, true)},
			{"CanCollide", Property::fromSimple<&BasePart::CanCollide>(true, true)},
			{"CastShadow", Property::fromSimple<&BasePart::CastShadow>(true, true)},
			{"CFrame", Property::fromSimple<&BasePart::CFrame>(true, true)},
			{"Color", Property::fromSimple<&BasePart::Color>(true, true)},
			{"Size", Property::fromSimple<&BasePart::Size>(true, true)},
			{"Transparency", Property::fromSimple<&BasePart::Transparency>(true, true)},
			{
				"Position",
				Property::fromReadWrite<glm::vec3>(
					[](Instance *self) { return self->Cast<BasePart>()->CFrame.Position; },
					[](Instance *self, glm::vec3 value) {
						auto part = self->Cast<BasePart>();
						part->CFrame = gargantuan::CFrame(value, part->CFrame.Rotation);
					}
				),
			}
		}
	};

	glm::mat4 BasePart::GetModelMatrix() {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), CFrame.Position);
		glm::mat4 rotation = CFrame.Rotation;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), Size);
		return translation * rotation * scale;
	}
} // namespace gargantuan
