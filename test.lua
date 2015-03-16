--Sample bot for Mysterious Path 3 (works on my Angelic Buster)

HookMove()
SetMoveXOff(25)
SetMoveDelay(400)
SetFaceDelay(100)
function AttackWait()
	KeyHoldFor(0x11, 10000)
	Wait(1000)
end
function JumpWait()
	KeyPress(0x12) --alt jump
	Wait(300)
end
while true do
	MoveX(17)
	KeyPress(0x12) --alt jump
	Rope(-1) --going up
	MoveX(285)
	JumpWait()
	FaceLeft()
	AttackWait()
	MoveX(379)
	KeyPress(0x12)
	Rope(-1)
	MoveX(855)
	JumpWait()
	FaceLeft()
	AttackWait()
	MoveX(1013)
	Rope(1) --going down
	MoveX(700)
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