Add new service
Create new file in 
```lua
data/lib/services/new_service.lua
```

```lua
local NewService = Notification:createBaseService()

NewService:configure({
    url = "",
    enabled = false,
    title = "Server"
})

function NewService:send(messageType, message)
    if not self.config.enabled then return false end
    
    Game.sendHttpRequest(
        self.config.url,
        "POST",
        { ["Content-Type"] = "application/json" },
        '{"text":"' .. message .. '"}',
        nil,
        self.config.timeout or Notification.defaults.timeout
    )
    return true
end

Notification:registerService("NewService", NewService)
```

Add in lib.lua:

```lua
dofile('data/lib/services/new_service.lua')
```
