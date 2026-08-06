local waterPipe = ItemEvent()

waterPipe.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	if math.random(3) == 1 then
		item:getPosition():sendMagicEffect(CONST_ME_POFF)
	else
		player:getPosition():sendMagicEffect(CONST_ME_POFF)
	end
	return true
end

waterPipe:id(2093, 2099)
waterPipe:register()
