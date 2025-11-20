-- Don't forget to add to talkactions.xml:
-- <talkaction words="!discord" script="discord_message.lua" />

function onSay(player, words, param)
    -- If the player's permission is lower than GOD (Normal player, Tutor, etc.), stop the function.
	if not player:getGroup():getAccess() then
		return true
	end

	if player:getAccountType() < ACCOUNT_TYPE_GOD then
		return false
	end

    -- Paste the FULL URL you copied from Discord.
    -- For example: https://discord.com/api/webhooks/1234567890123456789/aBcDeFgHiJkLmNoPqRsTuVwXyZ
    local webhookUrl = "https://discord.com/api/webhooks/..." 

    local message = "Hello from BlackTek Server!"

    Game.sendDiscordMessage(webhookUrl, MESSAGE_NORMAL, message)

    player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Discord message sent!")
    return false
end
