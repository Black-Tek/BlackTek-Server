local decayingTile = ItemEvent()

decayingTile.onStepOn = function(creature, item, position, fromPosition)
	item:transform(item.itemid + 1)
	item:decay()
	return true
end

decayingTile:id(293, 461)
decayingTile:register()
