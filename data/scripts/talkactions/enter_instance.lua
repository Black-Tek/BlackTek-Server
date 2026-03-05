local function enterInstance(player, param)
	local split = param:split(" ")
	local instanceId = tonumber(split[2])
	if not instanceId then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Usage: /instance enter <instanceId>")
		return false
	end

	player:setInstanceId(instanceId)
	player:refreshWorldView()
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Entered instance " .. instanceId .. ".")
	return false
end

local function exitInstance(player)
	player:setInstanceId(0)
	player:refreshWorldView()
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Exited to global instance (0).")
	return false
end

local function onSay(player, words, param)
	local loweredWords = words:lower()

	if loweredWords == "/exitinstance" then
		return exitInstance(player)
	end

	if loweredWords == "/enterinstance" then
		if param == "" then
			player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Usage: /enterinstance <instanceId>")
			return false
		end

		return enterInstance(player, "enter " .. param)
	end

	local loweredParam = param:lower()
	if loweredParam == "" then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Usage: /instance enter <instanceId> | /instance exit")
		return false
	end

	if loweredParam:sub(1, 4) == "exit" then
		return exitInstance(player)
	end

	if loweredParam:sub(1, 5) == "enter" then
		return enterInstance(player, param)
	end

	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Usage: /instance enter <instanceId> | /instance exit")
	return false
end

local instance = TalkAction("/instance", "/enterinstance", "/exitinstance")
function instance.onSay(player, words, param)
	return onSay(player, words, param)
end
instance:separator(" ")
instance:register()
