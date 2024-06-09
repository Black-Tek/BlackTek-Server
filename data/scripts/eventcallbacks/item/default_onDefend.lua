local ec = EventCallback

-- This Event is not triggered by healing.
-- This Event can be triggered by spells.
-- This event is called on items worn by the defender.
-- This Event is called everytime an item contributes to defending against any attack at all, damage or not.

-- criticalDamage and leechedDamage are true/false values, to determine if the attack was from a crit or a leech attack.

-- item.onDefend(item, player, creature, blockType combatType, origin, criticalDamage, leechedDamage)
ec.onDefend = function(self, defender, attacker, blockType, combatType, origin, criticalDamage, leechedDamage)
	-- default
	return
end

ec:register()
