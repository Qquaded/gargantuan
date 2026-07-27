local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local Workspace = game:GetService("Workspace")

local baseplate = Instance.new("Part")
baseplate.Name = "Baseplate"
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -20, 0)
baseplate.Size = Vector3.new(2000, 20, 2000)
baseplate.Transparency = 0
baseplate.Parent = Workspace

local hrp = Instance.new("Part")
hrp.Name = "HumanoidRootPart"
hrp.Size = Vector3.new(2, 4, 1)
hrp.Color = Color3.new(1, 1, 1)
hrp.Parent = Workspace

local part = Instance.new("Part")
part.Anchored = true
part.Color = Color3.new(0.5, 0.5, 0.5)
part.Position = Vector3.new(0, -2.5, 10)
part.Parent = Workspace
part.Size = Vector3.new(5, 5, 5)

Workspace.CurrentCamera.CameraType = Enum.CameraType.Scriptable

RunService.PreRender:Connect(function(dt: number)
	local walkDirection = Vector3.zero
	if UserInputService:IsKeyDown(Enum.KeyCode.W) then
		walkDirection += Vector3.new(0, 0, 0.5)
	end
	if UserInputService:IsKeyDown(Enum.KeyCode.S) then
		walkDirection -= Vector3.new(0, 0, 0.5)
	end
	if UserInputService:IsKeyDown(Enum.KeyCode.A) then
		walkDirection += Vector3.new(0.5, 0, 0)
	end
	if UserInputService:IsKeyDown(Enum.KeyCode.D) then
		walkDirection -= Vector3.new(0.5, 0, 0)
	end

	walkDirection *= 16 * dt
	hrp.Position += walkDirection

	local lookCFrame = CFrame.new((hrp.CFrame * CFrame.new(0, 20, -20)).Position, hrp.Position)
	Workspace.CurrentCamera.CFrame = lookCFrame
end)
