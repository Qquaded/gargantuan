local args = require("@std/args")

const parsed = args.parse("run", "runs the engine"):commands(
	args.command("example", "runs an example script"):args(
		args.positional("name", "name of the script, such as blocks.luau"),
		args.flag("--fresh", "whether to force a fresh recompilation")
	),
	args.command("studio", "runs the Gargantuan Studio project")
		:args(args.flag("--fresh", "whether to force a fresh recompilation"))
)
