local function onLogout(player)
	local playerId = player:getId()
	if nextUseStaminaTime[playerId] then
		nextUseStaminaTime[playerId] = nil
	end
	return true
end

-- Revscript registrations
local PlayerLogout = CreatureEvent("PlayerLogout")
function PlayerLogout.onLogout(...)
    return onLogout(...)
end
PlayerLogout:register()
