local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	local position = player:getPosition()
	position.z = position.z + 1
	player:teleportTo(position)
	return false
end

-- Revscript registrations
local down = TalkAction("/down")
function down.onSay(player, words, param)
    return onSay(player, words, param)
end
down:register()
