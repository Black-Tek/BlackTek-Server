local combat = Combat(HealingCombats.Recovery)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(159)
spell:name("Recovery")
spell:words("utura")
spell:level(50)
spell:mana(75)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(60000)
spell:groupCooldown(1000)
spell:vocation("paladin;true", "knight;true", "royal paladin;true", "elite knight;true")
spell:register()
