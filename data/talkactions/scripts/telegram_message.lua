-- Don't forget to add to talkactions.xml:
-- <talkaction words="!telegram" script="telegram_message.lua" />

function onSay(player, words, param)
    -- If the player's permission is lower than GOD (Normal player, Tutor, etc.), stop the function.
	if not player:getGroup():getAccess() then
		return true
	end

	if player:getAccountType() < ACCOUNT_TYPE_GOD then
		return false
	end

    local botToken = "<BOT_TOKEN_HERE>"
    local chatId = "<CHAT_ID_HERE>"

    local message = "Hello from BlackTek Server!"

    -- If param is provided, use it as message
    if param ~= "" then
        message = param
    end

    Game.sendTelegramMessage(botToken, chatId, message)

    player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Telegram message sent!")
    return false
end
