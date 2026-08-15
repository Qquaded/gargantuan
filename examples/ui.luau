const CoreGui = game:GetService("CoreGui")
local Workspace = game:GetService("Workspace")

CoreGui.Name = "CoreGui"

const half = Instance.new("Frame")
half.BackgroundColor3 = Color3.new(1, 0, 0)
half.Size = UDim2.fromScale(0.5, 1)
half.Name = "Frame"
half.BackgroundTransparency = 0.5

const centered = Instance.new("Frame")
centered.AnchorPoint = Vector2.new(0.5, 0.5)
centered.BackgroundColor3 = Color3.new(0, 1, 0)
centered.Size = UDim2.fromOffset(128, 128)
centered.Position = UDim2.fromScale(0.5, 0.5)
centered.Name = "Centered"

const screen = Instance.new("ScreenGui")
centered.Parent = half
half.Parent = screen
screen.Parent = CoreGui
centered.Name = "Screen"

print("Camera viewport size =", Workspace.CurrentCamera.ViewportSize)
print("Screen absolute size =", screen.AbsoluteSize)
print("Half absolute size =", half.AbsoluteSize)
print("Centered absolute size =", centered.AbsoluteSize)
