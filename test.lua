HookMove()
SetMoveXOff(20)
SetMoveDelay(300)
SetFaceDelay(100)
while true do
	MoveX(17)
	KeyPress(0x12) --alt jump
	Rope(-1) --going up
	MoveX(285)
	KeyPress(0x12)
	Wait(300)
	FaceLeft()
	KeyHoldFor(0x12, 1000)
	Wait(1000)
	MoveX(700)
	KeyHoldFor(0x12, 1000)
	Wait(1000)
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