local LoginEvent = CreatureEvent("DiscordHook")

--Discord webhook enums
--MESSAGE_NORMAL
--MESSAGE_ERROR;
--MESSAGE_LOG;
--MESSAGE_INFO;

local webhookLink = "https://discord.com/api/webhooks/1268439526154567752/mvJ3BYooIUA5laVKdX4KNWxYSat5Cx5RpccCAonSYJY_tsXthe_GY9mK8sAsQL0QxZL9"

function LoginEvent.onLogin(player)
  Game.queueDiscordMessage(webhookLink, MESSAGE_INFO, "Player: " .. player:getName() .. " has logged in")
  Game.sendDiscordWebhook()
  return true
end

LoginEvent:type("login")
LoginEvent:register()