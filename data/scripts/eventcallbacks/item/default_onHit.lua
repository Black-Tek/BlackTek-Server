local ec = EventCallback

-- This Event is not triggered by healing.
-- This Event can be triggered by spells.
-- This event is called on items worn by the attacker.
-- This Event is called everytime any attack does damage, and the item equipped has attack.

-- item.onHit(item, player, creature, combatType)
ec.onHit = function(self, attacker, target, combatType)
	-- default
	return
end

ec:register()
