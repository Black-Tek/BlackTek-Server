local combat = Combat(HealingCombats.CurePoison)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(29)
spell:name("Cure Poison")
spell:words("exana pox")
spell:level(10)
spell:mana(30)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(6000)
spell:groupCooldown(1000)
spell:vocation("sorcerer;true", "druid;true", "paladin;true", "knight;true", "master sorcerer;true", "elder druid;true", "royal paladin;true", "elite knight;true")
spell:register()
