function GetNumMobsOnPlatform(y)
	local mobs = maple.GetMobs()
	local count = 0

	for _, val in pairs(mobs) do
		if val.y == y then
			count = count + 1
		end
	end

	return count
end
function AmountOfItemByID(itemID)
	local inv = maple.GetInventory()
	local count = 0

	for _, tab in pairs(inv) do
		for _, item in pairs(tab) do
			if item.id == itemID then
				count = count + item.quantity
			end
		end
	end

	return count
end