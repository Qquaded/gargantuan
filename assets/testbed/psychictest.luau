local uis = game:GetService("UserInputService")
local w = game:GetService("Workspace")
local rs = game:GetService("RunService")

local fallingpart = Instance.new("Part")
fallingpart.Size = Vector3.new(5,5,5)
fallingpart.Position = Vector3.new(0,20,0)
fallingpart.Anchored = false
fallingpart.CanCollide = true
fallingpart.Shape = Enum.PartType.Ball
fallingpart.Parent = w

local groundpart = Instance.new("Part")
groundpart.Size = Vector3.new(20,1,20)
groundpart.Position = Vector3.new(0,-20,0)
groundpart.Anchored = true
groundpart.CanCollide = true
groundpart.Shape = Enum.PartType.Block
groundpart.Parent = w

local slope = Instance.new("Part")
slope.Size = Vector3.new(10,10,10)
slope.Position = Vector3.new(0,-15,0)
slope.Anchored = true
slope.CanCollide = true
slope.Shape = Enum.PartType.Wedge
slope.Parent = w

rs.PreRender:Connect(function(dt)
    print(fallingpart.Position) -- it works but like why not rendering?
end)
