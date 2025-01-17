local talk = TalkAction("/storage")

function talk.onSay(player, words, param)
	if not player or not player:getGroup():getAccess() or player:getAccountType() < ACCOUNT_TYPE_GOD then
		return false
	end
	
	local parameters = param:split(",")
	local playerName = (parameters[1] or ""):trim()
	local storageKey = tonumber(parameters[2]) or 0
	local storageValue = tonumber(parameters[3])
	
	if not playerName or not storageKey or #parameters <= 1 then
		player:sendTextMessage(MESSAGE_EVENT_ORANGE, "Insufficient parameters, usage: /storage playerName, key [, value]")
		return false
	end
	
	local checkedPlayer = Player(playerName)
	if not checkedPlayer then
		player:sendTextMessage(MESSAGE_EVENT_ORANGE, (string.format("Could not find player '%s'. Maybe is not online.", playerName)))
		player:getPosition():sendMagicEffect(CONST_ME_BUBBLES)
		return false
	end
	
	local msg
	if storageValue then
		checkedPlayer:setStorageValue(storageKey, storageValue)
		msg = string.format("Storage key '%s' is now set to '%d' for player '%s'.", storageKey, storageValue, checkedPlayer:getName())
	else
		storageValue = checkedPlayer:getStorageValue(storageKey)
		msg = string.format("Storage key '%s' is currently '%d' for player '%s'.", storageKey, storageValue, checkedPlayer:getName())
	end
	
	player:sendTextMessage(MESSAGE_EVENT_ORANGE, msg)
end

talk:separator(" ")
talk:register()
