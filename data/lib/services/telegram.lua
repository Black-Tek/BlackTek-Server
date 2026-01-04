local Telegram = Notification:createBaseService()

Telegram:configure({
    botToken = "",
    chatId = "",
    enabled = true
})

function Telegram:send(messageType, message)
    if not self.config.enabled or not self.config.botToken or not self.config.chatId then
        return false
    end
    
    local prefix = {
        normal = "",
        error = "ERROR: ",
        log = "LOG: ",
        info = "INFO: "
    }
    
    local text = (prefix[messageType] or "") .. message
    local url = string.format(
        "https://api.telegram.org/bot%s/sendMessage",
        self.config.botToken
    )
    
    local payload = json.encode({
        chat_id = self.config.chatId,
        text = text,
        parse_mode = "HTML"
    })
    
    Game.sendHttpRequest(
        url,
        "POST",
        { ["Content-Type"] = "application/json" },
        payload,
        function(success, code)
            if not success or code >= 400 then
                print("[Notification.Telegram] Error: " .. tostring(code))
            end
        end,
        self.config.timeout or Notification.defaults.timeout
    )
    
    return true
end

Notification:registerService("Telegram", Telegram)

