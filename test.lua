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

AttackAndLootFor = function (duration)
	KeyDown(0x11) --ctrl
	KeyDown(0x5A) --Z
	Wait(duration)
	KeyUp(0x5A)
	KeyUp(0x11)
end

UpRope = function ()
	HookMove()
	SetMove(0, -1)
	oY = GetY()
	Wait(100)
	while GetY() ~= oY do
		oY = GetY()
		Wait(100)
	end
	UnHookMove()
end