local sweetheartRing = ItemEvent()

sweetheartRing.onUse = function(player, item, fromPosition, target, toPosition, isHotkey)
	if item == player:getSlotItem(CONST_SLOT_RING) then
		player:getPosition():sendMagicEffect(CONST_ME_HEARTS)
		return true
	end
	return false
end

sweetheartRing:id(24324)
sweetheartRing:register()
