function GetMobX()
	repeat
		local closestX = 0
		local count = 0
		local dist = 9999999
		local charX = maple.GetChar().x
		local charY = maple.GetChar().y
		local mobs = maple.GetMobs()
		for _, mob in pairs(mobs) do
			diffY = math.abs(mob.y - charY)
			if diffY < 30 then
				diffX = math.abs(mob.x - charX)
				if diffX < dist then
					dist = diffX
					closestX = mob.x
					count = count + 1
				end
			end
		end
	until count ~= 0
	return closestX
end

function MoveToMob()
	while true do
		local mobX = GetMobX()
		local charX = maple.GetChar().x
		local diffX = math.abs(mobX - charX)
		if diffX < 50 then
			maple.SetMove(0, 0)
			break
		end
		maple.MoveTowardsX(mobX)
	end
end

function AttackWait()
	maple.KeyHoldFor(0x11, 1000)
	maple.Wait(1000)
end

maple.HookMove()
maple.SetMoveDelay(300)
while true do
	MoveToMob()
	AttackWait()
end
