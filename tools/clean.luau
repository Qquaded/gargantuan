local args = require("@std/args")

const parsed = args.parse("clean", "cleans up the build directory for a reconfiguration"):simple()
