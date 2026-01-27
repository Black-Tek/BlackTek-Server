local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	player:teleportTo(player:getTown():getTemplePosition())
	return false
end

-- Revscript registrations
local teleport_home = TalkAction("/t")
function teleport_home.onSay(player, words, param)
    return onSay(player, words, param)
end
teleport_home:register()
