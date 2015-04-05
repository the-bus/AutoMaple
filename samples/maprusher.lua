maps = {}
maps.nodes = {}
maps.id2num = {}

function string:split( inSplitPattern, outResults )
	if not outResults then
	outResults = { }
	end
	local theStart = 1
	local theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart, true )
	while theSplitStart do
		table.insert( outResults, string.sub( self, theStart, theSplitStart-1 ) )
		theStart = theSplitEnd + 1
		theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart, true )
	end
	table.insert( outResults, string.sub( self, theStart ) )
	return outResults
end

function maps:readFromFile()
	local count = 0
	for line in io.lines("mapcons.txt") do
		local a1 = string.split(line, "|")
		local a2 = string.split(a1[2], "`")
		local node = {}
		node.id = tonumber(a1[1])
		node.name = a1[3]
		node.portals = {}
		for _, portal in pairs(a2) do
			local a3 = string.split(portal, "^")
			table.insert(node, tonumber(a3[1]))
			table.insert(node.portals, {x=tonumber(a3[2]), y=tonumber(a3[3])})
		end
		table.insert(maps.nodes, node)
		count = count + 1
		maps.id2num[node.id] = count
	end
	for k, node in pairs(maps.nodes) do
		for l = 1, #node do
			local id = maps.id2num[node[l]]
			maps.nodes[k][l] = id or -1
		end
	end
end

function maps:getPath(startID, endID)
	return maple.GetPath(maps.nodes, maps.id2num[startID], maps.id2num[endID])
end

function maps:getPortal(startNum, endNum)
	local map = maps.nodes[startNum]
	for k, v in pairs(map) do
		if v == endNum then
			return map.portals[k]
		end
	end
end

function maps:getIDFromName(name)
	name = name:lower()
	for _, map in pairs(maps.nodes) do
		if map.name:lower():find(name, 1, true) then
			return map.id
		end
	end
end

maps:readFromFile()

function goThroughPortal()
	oMapID = maple.GetMapID()
	maple.EnableAutoPortal()
	while maple.GetMapID() == oMapID do
		maple.Wait(10)
	end
	maple.DisableAutoPortal()
end

function rushPath(path, yOff, finalX, finalY)
	for k = 2, #path - 1 do
		local portal = maps:getPortal(path[k], path[k+1])
		maple.SetSP(portal.x, portal.y - yOff)
		goThroughPortal()
	end
	if finalX and finalY then
		maple.SetSP(finalX, finalY)
		goThroughPortal()
		maple.UnHookSP()
	else
		maple.UnHookSP()
		goThroughPortal()
	end
end

function enterExitCS()
	maple.WaitForBreath()
	maple.Wait(1000)
	maple.SendPacket("5A 00 ?? ?? ?? 00 00")
	while maple.GetMapID() ~= 0 do
		maple.Wait(100)
	end
	maple.SendPacket("55 00")
end

function teleCS(x, y)
	maple.SetSP(x, y)
	enterExitCS()
	while maple.GetChar().x ~= x do
		maple.Wait(100)
	end
end

function rush(endID, yOff, finalX, finalY)
	local oMapID = maple.GetMapID()
	if endID == oMapID then
		return true
	end
	local path = maps:getPath(oMapID, endID)
	if #path == 0 then
		return false
	end
	maple.HookSP()
	local first = maps:getPortal(path[1], path[2])

	--maple.Teleport(first.x, first.y - yOff)

	teleCS(first.x, first.y - yOff)
	
	rushPath(path, yOff, finalX, finalY)
	return true
end

rush(maps:getIDFromName("mySterIous paTh 3"), 40)