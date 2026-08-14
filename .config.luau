return {
	luau = {
		languagemode = "strict",
		lint = { ["*"] = true },
		aliases = {
			-- Seal
			std = "./.seal/typedefs/std",
			extra = "./.seal/typedefs/extra",
			interop = "./.seal/typedefs/interop",
			-- Lest
			lest = "./.lest/core",
			-- Gargantuan
			tools = "./tools/libs",
		},
	},
	gargantuan = {
		buildDirectory = "build",
		binaryBasename = "gargantuan",

		classgenTargets = {
			classes = {
				input = "assets/classes",
				headerImport = "gargantuan/classes",
				headerOutput = "include/gargantuan/classes/generated",
				implOutput = "src/classes/generated",
			},
			services = {
				input = "assets/services",
				headerImport = "gargantuan/services",
				headerOutput = "include/gargantuan/services/generated",
				implOutput = "src/services/generated",
			},
		},
	},
}
