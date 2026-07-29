#include "gargantuan/classes/BasePart.hpp"
#include "gargantuan/datatypes/CFrame.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(
		BasePart,
		.Properties = {
			{"Anchored", Property::fromMember<&BasePart::Anchored>(true, true)},
			{"CanCollide", Property::fromMember<&BasePart::CanCollide>(true, true)},
			{"CastShadow", Property::fromMember<&BasePart::CastShadow>(true, true)},
			{"CFrame", Property::fromMember<&BasePart::CFrame>(true, true)},
			{"Color", Property::fromMember<&BasePart::Color>(true, true)},
			{"Size", Property::fromMember<&BasePart::Size>(true, true)},
			{"Transparency", Property::fromMember<&BasePart::Transparency>(true, true)},
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
	);

	glm::mat4 BasePart::GetModelMatrix() {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), CFrame.Position);
		glm::mat4 rotation = CFrame.Rotation;
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), Size);
		return translation * rotation * scale;
	}
} // namespace gargantuan
