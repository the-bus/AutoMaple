--Sample bot for Mysterious Path 3 (works on my Angelic Buster)
offset = 30
ropeOffset = offset + 30 --stop a bit more before ropes so we can jump while moving to get on ropes reliably
maple.HookMove()
maple.SetMoveXOff(offset)
maple.SetMoveDelay(400)
function AttackWait()
	maple.KeyHoldFor(0x11, 10000)
	maple.Wait(1000)
end
function JumpWait()
	maple.KeyPress(0x12) --alt jump
	maple.Wait(300)
end
while true do
	maple.KeySpam(0x5A) --z loot
	maple.MoveXOffNoStop(17, ropeOffset)
	maple.KeyDown(0x12)
	maple.RopeY(-193) --going up
	maple.KeyUp(0x12)
	maple.MoveX(285)
	JumpWait()
	maple.FaceLeft()
	maple.KeyUnSpam(0x5A)
	AttackWait()
	maple.KeySpam(0x5A)
	maple.MoveXOffNoStop(379, ropeOffset)
	maple.KeyDown(0x12)
	maple.RopeY(-553)
	maple.KeyUp(0x12)
	maple.MoveX(855)
	JumpWait()
	maple.FaceLeft()
	maple.KeyUnSpam(0x5A)
	AttackWait()
	maple.KeySpam(0x5A)
	maple.MoveXOffNoStop(1200, offset)
	maple.MoveX(700)
	maple.KeyUnSpam(0x5A)
	AttackWait()
end











--not used
maple.HookMove()
while true do
	maple.MoveX(maple.GetMobClosest().x)
end
function MoveToClosestRope()
	dist = 99999999
	local closest
	ropes = maple.GetRopes()
	charX = maple.GetChar().x
	charY = maple.GetChar().y
	for k, v in pairs(ropes) do
		temp = math.abs(charX - v[0].x)
		if temp < dist then
			dist = temp
			closest = k
		end
	end
	maple.MoveX(ropes[closest][0].x)
end
function GetMobsOnPlatform(y)
	mobs = maple.GetMobs()
	Count = 0
	for k, val in pairs(mobs) do
		if val.y == y then
			Count = Count + 1
		end
	end
	return Count
end
function CSTele(x, y)
	maple.HookSP()
	maple.WaitForBreath()
	maple.Wait(500)
	oMapID = maple.GetMapID() --original map ID
	maple.SetSP(x, y) --set the spawn point
	while maple.GetMapID() == oMapID do --wait until the map id changes (becomes invalid when the char enters CS)
		maple.KeyPress(0x44) --vk (virtual key) code for "d" (I put Cash Shop on "d")
		maple.Wait(50) --wait before checking again
	end
	while maple.GetMapID() ~= oMapID do --wait until the character has reentered the map
		--keep trying to exit
		maple.KeyPressNoHook(0x1B) --escape key code to exit CS
		maple.Wait(200) --wait a bit of time for the popup
		maple.KeyPressNoHook(0x20) --press space to exit the CS
		maple.Wait(200)
	end
	while maple.GetChar().x == 2147483647 do
		maple.Wait(50)
	end
	maple.Wait(500)
	maple.UnHookSP()
end