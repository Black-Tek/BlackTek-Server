local Slack = Notification:createBaseService()

Slack:configure({
    url = "",
    enabled = false,
    title = "Server"
})

Slack.colors = {
    normal = "#1BB8A5",
    error = "#FF0000",
    log = "#00A32F",
    info = "#FFB000"
}

function Slack:send(messageType, message)
    if not self.config.enabled or not self.config.url then
        return false
    end
    
    local color = self.colors[messageType] or self.colors.normal
    local title = self.config.title or messageType:upper()
    
    local payload = json.encode({
        attachments = {{
            color = color,
            title = title,
            text = message
        }}
    })
    
    Game.sendHttpRequest(
        self.config.url,
        "POST",
        { ["Content-Type"] = "application/json" },
        payload,
        function(success, code)
            if not success or code >= 400 then
                print("[Notification.Slack] Error: " .. tostring(code))
            end
        end,
        self.config.timeout or Notification.defaults.timeout
    )
    
    return true
end

Notification:registerService("Slack", Slack)

