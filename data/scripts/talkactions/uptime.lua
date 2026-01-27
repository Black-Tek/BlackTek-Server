local function onSay(player, words, param)
	local uptime = getWorldUpTime()

	local hours = math.floor(uptime / 3600)
	local minutes = math.floor((uptime - (3600 * hours)) / 60)
	player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Uptime: " .. hours .. " hours and " .. minutes .. " minutes.")
	return false
end

-- Revscript registrations
local uptime = TalkAction("!uptime")
function uptime.onSay(player, words, param)
    return onSay(player, words, param)
end
uptime:register()
