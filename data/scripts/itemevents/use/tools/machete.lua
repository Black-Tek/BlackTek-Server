local machete = ItemEvent()

machete.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	return onUseMachete(player, item, fromPosition, target, toPosition, isHotkey)
end

machete:id(2420, 2442)
machete:register()
