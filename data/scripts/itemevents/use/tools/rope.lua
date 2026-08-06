local rope = ItemEvent()

rope.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	return onUseRope(player, item, fromPosition, target, toPosition, isHotkey)
end

rope:id(2120, 7731)
rope:register()
