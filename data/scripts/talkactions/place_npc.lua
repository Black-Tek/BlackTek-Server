local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	if player:getAccountType() < ACCOUNT_TYPE_GOD then
		return false
	end

	local position = player:getPosition()
	local npc = Game.createNpc(param, position)
	if npc then
		npc:setMasterPos(position)
		position:sendMagicEffect(CONST_ME_MAGIC_RED)
	else
		player:sendCancelMessage("There is not enough room.")
		position:sendMagicEffect(CONST_ME_POFF)
	end
	return false
end

-- Revscript registrations
local place_npc = TalkAction("/s")
function place_npc.onSay(player, words, param)
    return onSay(player, words, param)
end
place_npc:separator(" ")
place_npc:register()
