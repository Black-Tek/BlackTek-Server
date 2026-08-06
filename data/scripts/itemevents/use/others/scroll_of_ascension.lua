local scrollOfAscension = ItemEvent()

scrollOfAscension.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	if math.random(10) > 1 then
		player:setMonsterOutfit("Demon", 30 * 10 * 1000)
	else
		player:setMonsterOutfit("Ferumbras", 30 * 10 * 1000)
	end
	item:transform(25428)
	item:getPosition():sendMagicEffect(CONST_ME_MAGIC_RED)
	item:decay()
	player:say("Magical sparks whirl around the scroll as you read it and then your appearance is changing.", TALKTYPE_MONSTER_SAY)
	return true
end

scrollOfAscension:id(25427)
scrollOfAscension:register()
