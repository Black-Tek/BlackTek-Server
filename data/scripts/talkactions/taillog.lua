local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	if player:getAccountType() < ACCOUNT_TYPE_GOD then
		return false
	end

	logCommand(player, words, param)

	local channel, countParam = param:match("^(%S+)%s*(%S*)$")
	if not channel then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Usage: /taillog <channel> [count]")
		return false
	end

	local count = tonumber(countParam) or 20
	local text = log.tail(channel, count)
	if not text then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Unknown log channel: " .. channel)
		return false
	end

	if text == "" then
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Log channel '" .. channel .. "' has no entries today.")
		return false
	end

	for line in text:gmatch("([^\n]+)") do
		player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, line)
	end

	return false
end

local taillog = TalkAction("/taillog")
function taillog.onSay(player, words, param)
    return onSay(player, words, param)
end
taillog:separator(" ")
taillog:register()
