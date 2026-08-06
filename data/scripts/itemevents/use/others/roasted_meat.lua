local freshMeat = ItemEvent()

freshMeat.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	if table.contains({1423, 1424, 1425}, target.itemid) then -- campfire
		item:transform(24843) -- roasted meat
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "You are roasting the fresh and bloody meat over the fire. Soon it smells appetizing.")
	end
	return true
end

freshMeat:id(24842)
freshMeat:register()
