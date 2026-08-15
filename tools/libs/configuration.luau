return {
	-- FIXME: blocked on Seal supporting .config.luau

	buildDirectory = "build",
	binaryBasename = "gargantuan",

	classgenTargets = {
		classes = {
			input = "data/classes",
			headerImport = "gargantuan/classes",
			headerOutput = "include/gargantuan/classes/generated",
			implOutput = "src/classes/generated",
		},
		services = {
			input = "data/services",
			headerImport = "gargantuan/services",
			headerOutput = "include/gargantuan/services/generated",
			implOutput = "src/services/generated",
		},
	},
}
