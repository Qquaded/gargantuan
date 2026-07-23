const Workspace = game:GetService("Workspace")

print("workspace", Workspace)

const FLOWERS = {
	Color3.new(0, 1, 1),
	Color3.new(0, 1, 0),
	Color3.new(1, 0.9, 0),
	Color3.new(1, 0, 1),
	Color3.new(1, 0.4, 0),
	Color3.new(0, 0, 1),
}

const TOTAL_FLOWERS = #FLOWERS
const RADIUS = 10
const AMPLITUDE = 0.5

local flowerParts = {}

local baseplate = Instance.new("Part")
baseplate.Name = "Baseplate"
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Position = Vector3.new(0, -50, 0)
baseplate.Size = Vector3.new(2000, 20, 2000)
baseplate.Transparency = 0
baseplate.Parent = Workspace

for index, flowerColor in FLOWERS do
	local flowerSoul = Instance.new("Part")
	flowerSoul.Color = flowerColor
	flowerSoul.Size = Vector3.new(2, 2, 2)
	flowerSoul.Parent = Workspace
	flowerParts[index] = flowerSoul
end

while task.wait() do
	for index, flowerSoul in flowerParts do
		local step = index / TOTAL_FLOWERS
		local angle = step * math.tau
		local x = math.cos(angle + os.clock()) * RADIUS
		local z = math.sin(angle + os.clock()) * RADIUS
		local y = math.sin(os.clock() + step) * AMPLITUDE
		flowerSoul.Position = Vector3.new(x, y, z)
	end
end
