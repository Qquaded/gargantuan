#include "gargantuan/classes/InputObject.hpp"
#include "gargantuan/datatypes/Vector3.hpp"
#include "gargantuan/scripting/Userdata.hpp"

namespace gargantuan {
	const std::unordered_map<Enums::ModifierKey, std::unordered_set<Enums::KeyCode>> MODIFIER_TO_KEYCODE = {
		{Enums::ModifierKey::Ctrl, {Enums::KeyCode::LeftControl, Enums::KeyCode::RightControl}},
		{Enums::ModifierKey::Alt, {Enums::KeyCode::LeftAlt, Enums::KeyCode::RightAlt}},
		{Enums::ModifierKey::Shift, {Enums::KeyCode::LeftShift, Enums::KeyCode::RightShift}},
		{Enums::ModifierKey::Meta, {Enums::KeyCode::LeftMeta, Enums::KeyCode::RightMeta}},
	};

	const InputObject::ClassDefinition InputObject::DEFINITION = {
		.Name = "InputObject",
		.Superclass = "Instance",
		.Properties =
			{
				G_UD_READONLY_PROP(InputObject, Delta, glm::vec3),
				G_UD_READONLY_PROP(InputObject, Position, glm::vec3),
				G_UD_READONLY_PROP(InputObject, KeyCode, Enums::KeyCode),
				G_UD_READONLY_PROP(InputObject, UserInputState, Enums::UserInputState),
				G_UD_READONLY_PROP(InputObject, UserInputType, Enums::UserInputType),
			},
		.Methods = {
			G_UD_METHOD(InputObject, IsModifierKeyDown),
		},
	};

	bool InputObject::IsModifierKeyDown(Enums::ModifierKey modifierKey) {
		return MODIFIER_TO_KEYCODE.at(modifierKey).contains(KeyCode);
	}
}
