const c, t, p = require("@scripts/classes").unpack()

return c({
	name = "LayerCollector",
	extends = "GuiBase2d",
	description = "Abstract class for containg 2D user interface elements.",
	abstract = true,
	since = "0.1.0",

	properties = {
		Enabled = {
			description = "Whether this instance should be rendered.",
			type = t.Bool,
			unmodified = "true",
		},
		ZIndexBehavior = {
			description = "Controls how GuiObject.ZIndex behaves on all descendants of this instance.",
			type = t.EnumItem("ZIndexBehavior"),
			unmodified = "Enums::ZIndexBehavior::Sibling",
		},
	},
})
