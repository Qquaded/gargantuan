#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/Log.hpp"
#include "gargantuan/classes/GuiBase2d.hpp"
#include "gargantuan/classes/Instance.hpp"
#include "gargantuan/datatypes/Vector2.hpp"
#include <memory>
#include <variant>

namespace gargantuan {
	GuiObject::GuiObject() {
		auto onBoundPropertyChanged = [this](std::monostate) { RecalculateAbsoluteBoundsAndChildren(); };
		GetPropertyChangedSignal("AnchorPoint")->Connect(onBoundPropertyChanged);
		GetPropertyChangedSignal("AutomaticSize")->Connect(onBoundPropertyChanged);
		GetPropertyChangedSignal("Position")->Connect(onBoundPropertyChanged);
		GetPropertyChangedSignal("Size")->Connect(onBoundPropertyChanged);
		GetPropertyChangedSignal("SizeConstraint")->Connect(onBoundPropertyChanged);
		AncestryChanged->Connect([this](std::tuple<std::shared_ptr<Instance>, std::shared_ptr<Instance>>) {
			RecalculateAbsoluteBoundsAndChildren();
		});
	}

	Rect GuiObject::CalculateAbsoluteBounds() {
		LOG_DEBUG(App, "Recomputing bounds for %s", GetFullName().c_str());
		auto parent = FindFirstAncestorWhichIsA("GuiBase2d");
		Rect parentBounds = {{0.0f, 0.0f}, {0.0f, 0.0f}};
		if (auto parentGui = std::dynamic_pointer_cast<GuiBase2d>(parent)) {
			auto parentPosition = parentGui->GetAbsolutePosition();
			parentBounds = {parentPosition, parentPosition + parentGui->GetAbsoluteSize()};
		}

		Vector2 size = CalculateConstrainedSize(parentBounds);
		Vector2 position = parentBounds.Min + (Position.AsScale() * parentBounds.GetSize() + Position.AsOffset()) -
						   (size * AnchorPoint);

		AbsoluteBounds = {position, position + size};
		SetAbsolutePosition(position);
		SetAbsoluteSize(size);
		return AbsoluteBounds;
	}

	Vector2 GuiObject::CalculateConstrainedSize(Rect parentBounds) const {
		switch (SizeConstraint) {
		case Enums::SizeConstraint::RelativeXY:
			return Size.AsScale() * parentBounds.GetSize() + Size.AsOffset();
		case Enums::SizeConstraint::RelativeXX: {
			auto constrained = Size.X.Scale * parentBounds.GetWidth() + Size.X.Offset;
			return {constrained, constrained};
		}
		case Enums::SizeConstraint::RelativeYY: {
			auto constrained = Size.Y.Scale * parentBounds.GetHeight() + Size.Y.Offset;
			return {constrained, constrained};
		}
		}
	}

	void GuiObject::RecalculateAbsoluteBoundsAndChildren() {
		CalculateAbsoluteBounds();
		for (const auto &child : GetChildren()) {
			if (auto guiChild = std::dynamic_pointer_cast<GuiObject>(child)) {
				guiChild->RecalculateAbsoluteBoundsAndChildren();
			}
		}
	}
}
