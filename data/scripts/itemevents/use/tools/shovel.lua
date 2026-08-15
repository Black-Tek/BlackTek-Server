local shovel = ItemEvent()

shovel.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	return onUseShovel(player, item, fromPosition, target, toPosition, isHotkey)
end

shovel:id(2554, 5710)
shovel:register()
