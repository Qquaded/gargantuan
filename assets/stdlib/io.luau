local ProcessService = game:GetService("ProcessService")

local io = {}

function io.write(...: string)
	ProcessService:WriteToStdout(...)
end

return table.freeze(io)
