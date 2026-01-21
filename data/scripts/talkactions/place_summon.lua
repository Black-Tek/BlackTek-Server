local function onSay(player, words, param)
	if not player:getGroup():getAccess() then
		return true
	end

	if player:getAccountType() < ACCOUNT_TYPE_GOD then
		return false
	end

	local position = player:getPosition()
	local monster = Game.createMonster(param, position)
	if monster then
		player:addSummon(monster)
		position:sendMagicEffect(CONST_ME_MAGIC_RED)
	else
		player:sendCancelMessage("There is not enough room.")
		position:sendMagicEffect(CONST_ME_POFF)
	end
	return false
end

-- Revscript registrations
local place_summon = TalkAction("/summon")
function place_summon.onSay(player, words, param)
    return onSay(player, words, param)
end
place_summon:separator(" ")
place_summon:register()
