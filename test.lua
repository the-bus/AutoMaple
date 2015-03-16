--Sample bot for Mysterious Path 3 (works on my Angelic Buster)

maple.HookMove()
maple.SetMoveXOff(30)
maple.SetMoveDelay(400)
maple.SetFaceDelay(100)
maple.SetRopePollDelay(400)
function AttackWait()
	maple.KeyHoldFor(0x11, 10000)
	maple.Wait(1000)
end
function JumpWait()
	maple.KeyPress(0x12) --alt jump
	maple.Wait(300)
end
maple.KeySpam(0x5A) --z key
while true do
	maple.MoveX(17)
	maple.KeyPress(0x12) --alt jump
	maple.Rope(-1) --going up
	maple.MoveX(285)
	JumpWait()
	maple.FaceLeft()
	AttackWait()
	maple.MoveX(379)
	maple.KeyPress(0x12)
	maple.Rope(-1)
	maple.MoveX(855)
	JumpWait()
	maple.FaceLeft()
	AttackWait()
	maple.MoveX(1013)
	maple.Rope(1) --going down
	maple.MoveX(700)
	AttackWait()
end











--not used
CSTele = function (x, y)
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