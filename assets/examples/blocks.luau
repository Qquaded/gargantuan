local Workspace = game:GetService("Workspace")
local RunService = game:GetService("RunService")

local baseplate = Instance.new("Part")
baseplate.Anchored = true
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -8, 0)
baseplate.Size = Vector3.new(1000, 16, 1000)
baseplate.Parent = Workspace

Workspace.CurrentCamera.CFrame = CFrame.new(Vector3.new(0, 15, 60), Vector3.new(0, 15, 0))

RunService.PreSimulation:Connect(function()
	local block = Instance.new("Part")
	block.Anchored = false
	block.CanCollide = true
	block.Color = Color3.fromHSV(math.random(), 1, 1)
	block.CFrame = CFrame.new(0, 20, 0)
		* CFrame.Angles(math.random() * math.pi, math.random() * math.pi, math.random() * math.pi)
	block.Position = Vector3.new(0, 25, 0)
	block.Size = Vector3.new(1, 1, 1)
	block.Parent = Workspace
end)
