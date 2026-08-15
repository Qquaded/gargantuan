#include "gargantuan/classes/LayerCollector.hpp"
#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/classes/Instance.hpp"
#include <memory>

namespace gargantuan {
	LayerCollector::LayerCollector() {
		BindDescendants([this](std::shared_ptr<Instance> instance) {
			if (auto it = std::dynamic_pointer_cast<GuiObject>(instance)) {
				GuiObjects.emplace(it);
			}
		});

		DescendantRemoved->Connect([this](std::shared_ptr<Instance> instance) {
			if (auto it = std::dynamic_pointer_cast<GuiObject>(instance); it && GuiObjects.contains(it)) {
				GuiObjects.erase(it);
			}
		});
	}
}
