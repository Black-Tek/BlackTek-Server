local LoginEvent = CreatureEvent("DiscordHook")

--Discord webhook enums
--MESSAGE_NORMAL
--MESSAGE_ERROR;
--MESSAGE_LOG;
--MESSAGE_INFO;

local webhookLink = "YOUR WEBHOOK HERE"

function LoginEvent.onLogin(player)
  Game.queueDiscordMessage(webhookLink, MESSAGE_INFO, "Player: " .. player:getName() .. " has logged in")
  Game.queueDiscordMessage(webhookLink, MESSAGE_ERROR, "Player: " .. player:getName() .. " has logged in")
  Game.queueDiscordMessage(webhookLink, MESSAGE_NORMAL, "Player: " .. player:getName() .. " has logged in")
  Game.queueDiscordMessage(webhookLink, MESSAGE_LOG, "Player: " .. player:getName() .. " has logged in")
  Game.sendDiscordWebhook()
  return true
end

LoginEvent:type("login")
LoginEvent:register()