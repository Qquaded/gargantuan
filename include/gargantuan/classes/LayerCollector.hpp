#pragma once

#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/classes/generated/LayerCollector.hpp"
#include "gargantuan/reflection/Enums.hpp"

#include <memory>
#include <unordered_set>

namespace gargantuan {
	G_ENUM(ZIndexBehavior, Sibling, Global);

	class LayerCollector : public GuiBase2d {
		I_LayerCollector;

		LayerCollector();
		std::unordered_set<std::shared_ptr<GuiObject>> GuiObjects;
	};
}
