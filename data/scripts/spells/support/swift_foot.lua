local combat = Combat(SupportCombats.SwiftFoot)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("support")
spell:id(134)
spell:name("Swift Foot")
spell:words("utamo tempo san")
spell:level(55)
spell:mana(400)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("paladin;true", "royal paladin;true")
spell:register()
