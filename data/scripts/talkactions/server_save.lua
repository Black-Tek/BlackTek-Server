local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end 
	
	if player:getAccountType() < ACCOUNT_TYPE_GOD then
		return false
	end
	
	saveData()
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Server Saved.")
	return false
end

-- Revscript registrations
local server_save = TalkAction("/save")
function server_save.onSay(player, words, param)
    return onSay(player, words, param)
end
server_save:register()
