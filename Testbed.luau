--!nolint
--!nocheck

print("runtime:", _RUNTIME)
print("runtime name:", _RUNTIME.name)
print("runtime url:", _RUNTIME.url)

game.Name = "Welcome To Hell"
print(game.Name)

-- local last = os.clock()
-- while true do
-- 	if os.clock() - last > 1 then
-- 		last = os.clock()
-- 		print("its been 1 second since last print:", last)
-- 	end
-- 	coroutine.yield()
-- end
