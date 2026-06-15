local combat = Combat(PartyCombats.HealParty)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return creature:addPartyCondition(combat, variant, PartyCombats.HealPartyCondition, 120)
end

spell:group("support")
spell:id(128)
spell:name("Heal Party")
spell:words("utura mas sio")
spell:level(32)
spell:mana(0)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
