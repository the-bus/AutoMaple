--Sample bot for Mysterious Path 3 (works on my Angelic Buster)

maple.HookMove()
maple.SetMoveXOff(30)
maple.SetMoveDelay(400)
maple.SetFaceDelay(100)
--maple.SetRopePollDelay(400) --not needed when using RopeY
function AttackWait()
	maple.KeyHoldFor(0x11, 10000)
	maple.Wait(1000)
end
function JumpWait()
	maple.KeyPress(0x12) --alt jump
	maple.Wait(300)
end
while true do
maple.KeySpam(0x5A)
	maple.MoveX(17)
	maple.KeyDown(0x12)
	maple.RopeY(-193) --going up
	maple.KeyUp(0x12)
	maple.MoveX(285)
	JumpWait()
	maple.FaceLeft()
maple.KeyUnSpam(0x5A)
	AttackWait()
maple.KeySpam(0x5A)
	maple.MoveX(379)
	maple.KeyDown(0x12)
	maple.RopeY(-553)
	maple.KeyUp(0x12)
	maple.MoveX(855)
	JumpWait()
	maple.FaceLeft()
maple.KeyUnSpam(0x5A)
	AttackWait()
maple.KeySpam(0x5A)
	maple.MoveX(1013)
	maple.RopeY(-253) --going down
	maple.MoveX(700)
maple.KeyUnSpam(0x5A)
	AttackWait()
end











--not used
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
	HookSP()
	WaitForBreath()
	Wait(500)
	oMapID = GetMapID() --original map ID
	SetSP(x, y) --set the spawn point
	while GetMapID() == oMapID do --wait until the map id changes (becomes invalid when the char enters CS)
		KeyPress(0x44) --vk (virtual key) code for "d" (I put Cash Shop on "d")
		Wait(50) --wait before checking again
	end
	while GetMapID() ~= oMapID do --wait until the character has reentered the map
		--keep trying to exit
		KeyPressNoHook(0x1B) --escape key code to exit CS
		Wait(200) --wait a bit of time for the popup
		KeyPressNoHook(0x20) --press space to exit the CS
		Wait(200)
	end
	while GetX() == 2147483647 do
		Wait(50)
	end
	Wait(500)
	UnHookSP()
end