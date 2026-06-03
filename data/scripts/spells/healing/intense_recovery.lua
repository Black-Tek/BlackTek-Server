local combat = Combat(HealingCombats.IntenseRecovery)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(160)
spell:name("Intense Recovery")
spell:words("utura gran")
spell:level(100)
spell:mana(165)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(60000)
spell:groupCooldown(1000)
spell:vocation("paladin;true", "knight;true", "royal paladin;true", "elite knight;true")
spell:register()
