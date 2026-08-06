local trapReset = ItemEvent()

trapReset.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	item:transform(item:getId() - 1)
	fromPosition:sendMagicEffect(CONST_ME_POFF)
	return true
end

trapReset:id(2579)
trapReset:register()
