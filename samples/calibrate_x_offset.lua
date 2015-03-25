--this script should be run from a place where you can move a lot to the right
--and on a flat platform
maple.HookMove()
maple.SetMoveDelay(500)
iters = 3
sum = 0
for i = 1, iters do
	charX = maple.GetChar().x
	targetX = charX + 100
	maple.MoveXOff(targetX, 0)
	off = maple.GetChar().x - targetX
	sum = sum + off
end
maple.Message(string.format("Your char's move x offset is about %d", math.ceil(sum / iters)))
maple.UnHookMove()