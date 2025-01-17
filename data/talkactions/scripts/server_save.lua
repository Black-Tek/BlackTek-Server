function onSay(player, words, param)
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
