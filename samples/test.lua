--Sample bot for Mysterious Path 3 (works on my Angelic Buster, not super reliable)
maple.AutoHP(0x21, 4500)
offset = 30
ropeOffset = offset + 40 --stop a bit more before ropes so we can jump while moving to get on ropes reliably
maple.HookMove()
maple.SetMoveXOff(offset)
maple.SetMoveDelay(400)
maple.SetTimeout(10)
function AttackWait()
	maple.KeyHoldFor(0x11, 10000)
	maple.Wait(1000)
end
function JumpWait()
	maple.KeyPress(0x12) --alt jump
	maple.Wait(300)
end
function JumpWaitRope()
	maple.KeyDown(0x12)
	maple.Wait(40)
end
function Restart()
	maple.SetMove(0, 0)
	maple.ResetKeys()
	maple.Teleport(700, 107)
	maple.Wait(500)
end
function main()
	--maple.KeyPress(0x2E) --buff Del key
	--maple.Wait(1000) --wait 1 second
	maple.KeySpam(0x5A) --z loot
	if maple.MoveXOffNoStop(17, ropeOffset) == false then
		return Restart()
	end
	JumpWaitRope()
	if maple.RopeY(-193) == false then --going up
		return Restart()
	end
	maple.KeyUp(0x12)
	if maple.MoveX(285) == false then
		return Restart()
	end
	JumpWait()
	maple.FaceLeft()
	maple.KeyUnSpam(0x5A)
	AttackWait()
	maple.KeySpam(0x5A)
	if maple.MoveXOffNoStop(379, ropeOffset) == false then
		return Restart()
	end
	JumpWaitRope()
	if maple.RopeY(-553) == false then
		return Restart()
	end
	maple.KeyUp(0x12)
	if maple.MoveX(855) == false then
		return Restart()
	end
	JumpWait()
	maple.FaceLeft()
	maple.KeyUnSpam(0x5A)
	AttackWait()
	maple.KeySpam(0x5A)
	if maple.MoveXOffNoStop(1200, offset) == false then
		return Restart()
	end
	if maple.MoveX(700) == false then
		return Restart()
	end
	maple.KeyUnSpam(0x5A)
	AttackWait()
end

while true do
	main()
end