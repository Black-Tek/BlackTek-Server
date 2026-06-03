local combat = Combat(SupportCombats.BloodRage)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("support")
spell:id(133)
spell:name("Blood Rage")
spell:words("utito tempo")
spell:level(60)
spell:mana(290)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
