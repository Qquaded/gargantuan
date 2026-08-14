local args = require("@std/args")

const parsed = args.parse("fresh", "recompiles Gargantuan, then runs the specified target"):commands()
