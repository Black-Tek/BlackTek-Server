local ec = EventCallback

-- This Event is not triggered by healing.
-- This Event can be triggered by spells.
-- This event is called on items worn by the attacker.
-- This Event is called everytime any attack takes place damage or not, and the item equipped has attack.

-- item.onAttack(item, player, creature, combatType)
ec.onAttack = function(self, attacker, target, combatType)
	-- default
	return
end

ec:register()
