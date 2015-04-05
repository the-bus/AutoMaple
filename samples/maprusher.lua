maps = {}
maps.nodes = {}
maps.id2num = {}

function string:split( inSplitPattern, outResults )
	if not outResults then
	outResults = { }
	end
	local theStart = 1
	local theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
	while theSplitStart do
		table.insert( outResults, string.sub( self, theStart, theSplitStart-1 ) )
		theStart = theSplitEnd + 1
		theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
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
			local a3 = string.split(portal, "%^")
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
	for _, map in pairs(maps.nodes) do
		if map.name == name then
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

function rush(endID, yOff, finalX, finalY)
	local path = maps:getPath(maple.GetMapID(), endID)
	maple.HookSP()
	for k = 2, #path - 1 do
		local portal = maps:getPortal(path[k], path[k+1])
		if k == 2 then
			local previous = maps:getPortal(path[k-1], path[k])
			maple.Teleport(previous.x, previous.y - yOff)
		end
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

rush(maps:getIDFromName("Zipangu: Mushroom Shrine"), 40)