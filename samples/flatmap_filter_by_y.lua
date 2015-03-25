function GetMobX()
	repeat
		closestX = 0
		count = 0
		dist = 9999999
		charX = maple.GetChar().x
		charY = maple.GetChar().y
		mobs = maple.GetMobs()
		for k, mob in pairs(mobs) do
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
		mobX = GetMobX()
		charX = maple.GetChar().x
		diffX = math.abs(mobX - charX)
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
