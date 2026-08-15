local pick = ItemEvent()

pick.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	return onUsePick(player, item, fromPosition, target, toPosition, isHotkey)
end

pick:id(2553)
pick:register()
