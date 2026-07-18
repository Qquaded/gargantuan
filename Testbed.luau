--!nolint
--!nocheck

const Workspace = game:GetService("Workspace")

const CUBE_CYCLE_DURATION = 1

const baseplate = Instance.new("Part")
baseplate.Position = Vector3.new(0, -30, 0)
baseplate.Color = Color3.fromHSV(0, 0, 0.5)
baseplate.Size = Vector3.new(1000, 20, 1000)

const cube = Instance.new("Part")
cube.Position = Vector3.new(0, 10, 0)
cube.Size = Vector3.new(5, 5, 5)

while true do
	local cubeHue = os.clock() % CUBE_CYCLE_DURATION / CUBE_CYCLE_DURATION
	cube.Color = Color3.fromHSV(1, 1, cubeHue)
end
