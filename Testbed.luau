--!nolint
--!nocheck

print("runtime:", _RUNTIME)
print("runtime name:", _RUNTIME.name)
print("runtime url:", _RUNTIME.url)

local test = Instance.new("Part")
-- FIXME: setting properties to test segfaults the program?????
-- test.Name = "Test"
-- test.Parent = game

for _, child in game:GetChildren() do
	print("Child named", child.Name, "of class", child.ClassName)
end

while true do
	game.Cube.Transparency = os.clock() % 1
	coroutine.yield()
end
