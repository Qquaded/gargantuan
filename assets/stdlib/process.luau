local ProcessService = game:GetService("ProcessService")

local process = {}

function process.exit(exitCode: number): never
	return ProcessService:ExitAsync(exitCode)
end

return table.freeze(process)
