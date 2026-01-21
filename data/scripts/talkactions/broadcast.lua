local function onSay(player, words, param)
	if not player:hasFlag(PlayerFlag_CanBroadcast) then
		return true
	end

	print("> " .. player:getName() .. " broadcasted: \"" .. param .. "\".")
	for _, targetPlayer in ipairs(Game.getPlayers()) do
		targetPlayer:sendPrivateMessage(player, param, TALKTYPE_BROADCAST)
	end
	return false
end

-- Revscript registrations
local broadcast = TalkAction("/B")
function broadcast.onSay(player, words, param)
    return onSay(player, words, param)
end
broadcast:separator(" ")
broadcast:register()
