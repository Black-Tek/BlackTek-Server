local combat = Combat(SupportCombats.Charge)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("support")
spell:id(131)
spell:name("Charge")
spell:words("utani tempo hur")
spell:level(25)
spell:mana(100)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
