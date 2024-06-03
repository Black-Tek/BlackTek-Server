local ec = EventCallback

-- This Event is not triggered by healing.
-- This Event can be triggered by spells.
-- This event is called on items worn by the defender.
-- This Event is called everytime an item contributes to defending against any attack at all, damage or not.

-- item.onDefendAttack(item, player, creature, combatType)
ec.onDefendAttack = function(self, defender, attacker, combatType)
	-- default
	return
end

ec:register()
