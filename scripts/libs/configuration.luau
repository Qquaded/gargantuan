-- FIXME: blocked on Seal supporting .config.luau

return {
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
}
