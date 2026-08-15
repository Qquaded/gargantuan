local process = require("@std/process")
local vars = require("@std/env/vars")

const function countCpus(): number
	-- Windows
	const envNumProcessors = vars.get("NUMBER_OF_PROCESSORS")
	if envNumProcessors then
		const numProcessors = tonumber(envNumProcessors)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	-- Linux
	const nprocResult = process.run({ program = "nproc", args = { "--all" } })
	if nprocResult.ok then
		const numProcessors = tonumber(nprocResult.stdout)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	-- MacOS
	const sysResult = process.run({ program = "sysctl", args = { "-n", "hw.ncpu" } })
	if sysResult.ok then
		const numProcessors = tonumber(sysResult.stdout)
		if numProcessors and numProcessors > 0 then
			return numProcessors
		end
	end

	return 1
end

return countCpus
