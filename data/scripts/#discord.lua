local LoginEvent = CreatureEvent("DiscordHook")

--Discord webhook enums
--MESSAGE_NORMAL
--MESSAGE_ERROR;
--MESSAGE_LOG;
--MESSAGE_INFO;

local webhookLink = "TOKEN HERE"

function LoginEvent.onLogin(player)
  Game.sendDiscordMessage(webhookLink, MESSAGE_INFO, "Player: " .. player:getName() .. " has logged in")
  Game.sendDiscordMessage(webhookLink, MESSAGE_ERROR, "Player: " .. player:getName() .. " has logged in")
  Game.sendDiscordMessage(webhookLink, MESSAGE_NORMAL, "Player: " .. player:getName() .. " has logged in")
  Game.sendDiscordMessage(webhookLink, MESSAGE_LOG, "Player: " .. player:getName() .. " has logged in")
  return true
end

LoginEvent:type("login")
LoginEvent:register()
