#pragma once

#include "gargantuan/classes/generated/GuiObject.hpp"
#include "gargantuan/datatypes/Rect.hpp"
#include "gargantuan/reflection/Enums.hpp"

namespace gargantuan {
	G_ENUM(GuiState, Idle, Hover, Press, NonInteractable);
	G_ENUM(InputSink, None, Activate, All = 100);
	G_ENUM(SizeConstraint, RelativeXX, RelativeXY, RelativeYY);
	G_ENUM(AutomaticSize, None, X, Y, XY);

	class GuiObject : public GuiBase2d {
		I_GuiObject;

		GuiObject();

		Rect AbsoluteBounds = {0.0f, 0.0f};
		Rect CalculateAbsoluteBounds();
		Vector2 CalculateConstrainedSize(Rect parentBounds) const;
		void RecalculateAbsoluteBoundsAndChildren();
	};
}
