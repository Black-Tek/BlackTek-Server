local Discord = Notification:createBaseService()

Discord:configure({
    url = "",
    enabled = false,
    title = "Server"
})

Discord.colors = {
    normal = 1815333,
    error = 16711680,
    log = 41727,
    info = 16762880
}

function Discord:send(messageType, message)
    if not self.config.enabled or not self.config.url then
        return false
    end
    
    local color = self.colors[messageType] or self.colors.normal
    local title = self.config.title or messageType:upper()
    
    local payload = json.encode({
        content = json.null,
        embeds = {{
            title = title,
            description = message,
            color = color
        }}
    })
    
    Game.sendHttpRequest(
        self.config.url,
        "POST",
        { ["Content-Type"] = "application/json" },
        payload,
        function(success, code)
            if not success or code >= 400 then
                print("[Notification.Discord] Error: " .. tostring(code))
            end
        end,
        self.config.timeout or Notification.defaults.timeout
    )
    
    return true
end

Notification:registerService("Discord", Discord)

