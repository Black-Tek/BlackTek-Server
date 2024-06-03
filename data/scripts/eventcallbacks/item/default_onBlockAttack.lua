local ec = EventCallback

-- This Event is not triggered by healing.
-- This Event can be triggered by spells.
-- This event is called on items worn by the defender.
-- This Event is called everytime any attack is blocked by block_armor or block_defense, on any item that has contributed to the blocked attack.

-- item.onBlockAttack(item, player, creature, combatType)
ec.onBlockAttack = function(self, defender, attacker, combatType)
	-- default
	return
end

ec:register()
