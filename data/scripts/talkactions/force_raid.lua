local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	if player:getAccountType() < ACCOUNT_TYPE_GAMEMASTER then
		return false
	end

	logCommand(player, words, param)

	local returnValue = Game.startRaid(param)
	if returnValue ~= RETURNVALUE_NOERROR then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, Game.getReturnMessage(returnValue))
	else
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Raid started.")
	end

	return false
end

-- Revscript registrations
local force_raid = TalkAction("/raid")
function force_raid.onSay(player, words, param)
    return onSay(player, words, param)
end
force_raid:separator(" ")
force_raid:register()
