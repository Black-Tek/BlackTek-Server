local explosivePresent = ItemEvent()

explosivePresent.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	player:say("KABOOOOOOOOOOM!", TALKTYPE_MONSTER_SAY)
	player:getPosition():sendMagicEffect(CONST_ME_FIREAREA)
	item:remove()
	return true
end

explosivePresent:id(9074)
explosivePresent:register()
