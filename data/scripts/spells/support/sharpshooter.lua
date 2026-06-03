local combat = Combat(SupportCombats.Sharpshooter)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("support", "healing")
spell:id(135)
spell:name("Sharpshooter")
spell:words("utito tempo san")
spell:level(60)
spell:mana(450)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(10000, 10000)
spell:vocation("paladin;true", "royal paladin;true")
spell:register()
