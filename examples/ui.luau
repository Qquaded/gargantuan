const CoreGui = game:GetService("CoreGui")

const frame = Instance.new("Frame")
frame.BackgroundColor3 = Color3.new(1, 0, 0)
frame.Size = UDim2.fromScale(0.5, 1)

const screen = Instance.new("ScreenGui")
frame.Parent = screen
screen.Parent = CoreGui
