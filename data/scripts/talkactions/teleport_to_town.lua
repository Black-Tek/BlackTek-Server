local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	local town = Town(param) or Town(tonumber(param))
	if town then
		player:teleportTo(town:getTemplePosition())
	else
		player:sendCancelMessage("Town not found.")
	end
	return false
end

-- Revscript registrations
local teleport_to_town = TalkAction("/town")
function teleport_to_town.onSay(player, words, param)
    return onSay(player, words, param)
end
teleport_to_town:separator(" ")
teleport_to_town:register()
