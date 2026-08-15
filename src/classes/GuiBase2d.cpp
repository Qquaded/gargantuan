#include "gargantuan/classes/GuiBase2d.hpp"
#include "gargantuan/classes/GuiObject.hpp"
#include <variant>

namespace gargantuan {
	GuiBase2d::GuiBase2d() {
		auto recomputeGuiObjects = [this](std::monostate) {
			for (const auto &child : GetChildren()) {
				if (auto guiChild = std::dynamic_pointer_cast<GuiObject>(child)) {
					guiChild->RecalculateAbsoluteBoundsAndChildren();
				}
			}
		};

		GetPropertyChangedSignal("AbsoluteSize")->Connect(recomputeGuiObjects);
		GetPropertyChangedSignal("AbsolutePosition")->Connect(recomputeGuiObjects);
	};
}
