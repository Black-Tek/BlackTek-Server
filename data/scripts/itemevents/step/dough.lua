local campfireField = ItemEvent()

campfireField.onAddItem = function(moveitem, tileitem, position)
	if moveitem:getId() == 2693 then
		moveitem:transform(2689)
		position:sendMagicEffect(CONST_ME_HITBYFIRE)
	elseif moveitem:getId() == 6277 then
		moveitem:transform(2687, 12)
		position:sendMagicEffect(CONST_ME_HITBYFIRE)
	end
	return true
end

campfireField:id(1786, 1788, 1790, 1792)
campfireField:tileItem(true)
campfireField:register()
