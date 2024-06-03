local ec = EventCallback

-- This Event is not triggered by healing.
-- This Event can be triggered by spells.
-- This event is called on items worn by the attacker.
-- This Event is called everytime any attack takes place, the item has attack and is equipped, but the attack does not damage due to a blocked (block_defense or block_armor) hit.

-- item.onBlocked(item, player, creature, combatType)
ec.onBlocked = function(self, attacker, target, combatType)
	-- default
	return
end

ec:register()
