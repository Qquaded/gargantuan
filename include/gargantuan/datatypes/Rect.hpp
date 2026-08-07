#pragma once

#include "gargantuan/datatypes/Vector2.hpp"
#include "gargantuan/scripting/Userdata.hpp"

#include <glm/glm.hpp>

namespace gargantuan {
	G_USERDATA_DECL(
		Rect,

		Rect();
		Rect(Vector2 min, Vector2 max);
		Rect(float minX, float minY, float maxX, float maxY);

		Vector2 Min{0.0f, 0.0f};
		Vector2 Max{0.0f, 0.0f};

		float GetWidth() const;
		float GetHeight() const;
		Vector2 GetSize() const;
	);
}
