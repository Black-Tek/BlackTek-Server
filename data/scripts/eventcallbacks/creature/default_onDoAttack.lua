local ec = EventCallback

-- This Event is not triggered by healing.
-- This Event can be triggered by spells.
-- This event is called on items worn by the attacker.
-- This Event is called everytime any attack takes place damage or not

-- criticalDamage and leechedDamage are true/false values, to determine if the attack was from a crit or a leech attack.

-- creature.onDoAttack(creature, creature, blockType, combatType, origin, criticalDamage, leechedDamage)
ec.onDoAttack = function(self, target, blockType, combatType, origin, criticalDamage, leechedDamage)
	-- default
	return
end

ec:register()
