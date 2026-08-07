#include "gargantuan/datatypes/Rect.hpp"
#include "gargantuan/scripting/Userdata.hpp"
#include "gargantuan/scripting/UserdataTag.hpp"

namespace gargantuan {
	G_USERDATA_IMPL(
		Rect,
		.Tag = UserdataTag::Rect,
		.Type = "Rect",
		.Properties = {
			{"Min", Property::fromReadonlyMember<&Rect::Min>()},
			{"Max", Property::fromReadonlyMember<&Rect::Max>()},
			{"Size", Property::fromRead([](Rect *self) { return self->GetSize(); })},
			{"Width", Property::fromRead([](Rect *self) { return self->GetWidth(); })},
			{"Height", Property::fromRead([](Rect *self) { return self->GetHeight(); })},
		}
	)

	Rect::Rect() {};
	Rect::Rect(Vector2 min, Vector2 max) : Min(min), Max(max) {};
	Rect::Rect(float minX, float minY, float maxX, float maxY) : Min(minX, minY), Max(maxX, maxY) {};

	float Rect::GetWidth() const {
		return Max.GetX() - Min.GetX();
	}

	float Rect::GetHeight() const {
		return Max.GetY() - Min.GetY();
	}

	Vector2 Rect::GetSize() const {
		return {GetWidth(), GetHeight()};
	}
}
