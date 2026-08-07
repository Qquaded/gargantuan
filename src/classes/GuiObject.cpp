#include "gargantuan/classes/GuiObject.hpp"
#include "gargantuan/reflection/InstanceClassRegistry.hpp"

namespace gargantuan {
	G_INSTANCE_ABSTRACT_IMPL(
		GuiObject,
		.Superclass = "GuiBase2d",
		.Properties = {
			{"AnchorPoint", Property::fromMember<&GuiObject::AnchorPoint>()},
			{"BackgroundColor3", Property::fromMember<&GuiObject::BackgroundColor3>()},
			{"BackgroundTransparency", Property::fromMember<&GuiObject::BackgroundTransparency>()},
			{"ClipsDescendants", Property::fromMember<&GuiObject::ClipsDescendants>()},
			{"GuiState", Property::fromMember<&GuiObject::GuiState>()},
			{"InputSink", Property::fromMember<&GuiObject::InputSink>()},
			{"Interactable", Property::fromMember<&GuiObject::Interactable>()},
			{"LayoutOrder", Property::fromMember<&GuiObject::LayoutOrder>()},
			{"Position", Property::fromMember<&GuiObject::Position>()},
			{"Rotation", Property::fromMember<&GuiObject::Rotation>()},
			{"Selectable", Property::fromMember<&GuiObject::Selectable>()},
			{"SelectionImageObject", Property::fromMember<&GuiObject::SelectionImageObject>()},
			{"SelectionOrder", Property::fromMember<&GuiObject::SelectionOrder>()},
			{"Size", Property::fromMember<&GuiObject::Size>()},
			{"SizeConstaint", Property::fromMember<&GuiObject::SizeConstaint>()},
			{"Visible", Property::fromMember<&GuiObject::Visible>()},
			{"ZIndex", Property::fromMember<&GuiObject::ZIndex>()},
		}
	);

	Rect GuiObject::CalculateAbsoluteBounds() {
		if (!AbsoluteBoundsDirty) return AbsoluteBounds;

		auto parent = FindFirstAncestorWhichIsA("GuiBase2d");
		if (parent) {
			// ???????
		}

		AbsoluteBoundsDirty = false;
		return AbsoluteBounds;
	}
}
