Notification = {
    services = {},
    defaults = {
        timeout = 10,
        retries = 3,
        retryDelay = 1000
    }
}

function Notification:registerService(name, service)
    service.config = service.config or { enabled = false }
    self.services[name] = service
    self[name] = service
end

function Notification:broadcast(messageType, message)
    for _, service in pairs(self.services) do
        if service.config and service.config.enabled then
            service:send(messageType, message)
        end
    end
end

function Notification:createBaseService()
    local service = {
        config = { enabled = false }
    }
    
    function service:configure(cfg)
        for k, v in pairs(cfg) do
            self.config[k] = v
        end
    end
    
    function service:sendMessage(msg) return self:send("normal", msg) end
    function service:sendError(msg) return self:send("error", msg) end
    function service:sendLog(msg) return self:send("log", msg) end
    function service:sendInfo(msg) return self:send("info", msg) end
    
    function service:sendWithRetry(messageType, message, attempt)
        attempt = attempt or 1
        local maxRetries = self.config.retries or Notification.defaults.retries
        local retryDelay = self.config.retryDelay or Notification.defaults.retryDelay
        
        local success = self:send(messageType, message)
        
        if not success and attempt < maxRetries then
            addEvent(function()
                self:sendWithRetry(messageType, message, attempt + 1)
            end, retryDelay)
        end
        
        return success
    end
    
    return service
end

