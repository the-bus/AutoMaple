--Sample bot for Mysterious Path 3 (works on my Angelic Buster, not super reliable)
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