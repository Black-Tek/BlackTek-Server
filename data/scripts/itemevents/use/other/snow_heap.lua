local snowHeap = ItemEvent()

snowHeap.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	Game.createItem(2111, 1, item:getPosition())
	return true
end

snowHeap:id(486)
snowHeap:register()
