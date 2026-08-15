local args = require("@std/args")
local configuration = require("@tools/configuration")
local countCpus = require("@tools/utils/countCpus")
local process = require("@std/process")

const parsed = args.parse("build", "compiles the engine"):simple(
	args.named("--jobs", "number of jobs passed to CMake, defaults to the number of CPUs on this system"):aliases("-j"),
	args.flag("--no-classgen", "skip recompilation of instance classes")
)

do
	if not parsed.flags["no-classgen"] then
		const classgenResult = process.run({ program = "seal", args = { "tools/classgen.luau" }, stdio = "Inherit" })
		if not classgenResult.ok then
			error("Failed to generate instance classes")
		end
	end

	const jobs = tonumber(parsed:get("jobs")) or countCpus()
	const cmakeResult = process.run({
		program = "cmake",
		args = { "--build", configuration.buildDirectory, "-j", tostring(jobs) },
		stdio = "Inherit",
	})

	if not cmakeResult.ok then
		error("Failed to run CMake")
	end

	process.exit(0)
end
