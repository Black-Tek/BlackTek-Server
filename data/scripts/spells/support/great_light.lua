local combat = Combat(SupportCombats.GreatLight)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("support")
spell:id(11)
spell:name("Great Light")
spell:words("utevo gran lux")
spell:level(13)
spell:mana(60)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("sorcerer;true", "druid;true", "paladin;true", "knight;true", "master sorcerer;true", "elder druid;true", "royal paladin;true", "elite knight;true")
spell:register()
