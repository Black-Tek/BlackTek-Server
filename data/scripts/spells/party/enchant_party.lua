local combat = Combat(PartyCombats.EnchantParty)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return creature:addPartyCondition(combat, variant, PartyCombats.EnchantPartyCondition, 120)
end

spell:group("support")
spell:id(129)
spell:name("Enchant Party")
spell:words("utori mas sio")
spell:level(32)
spell:mana(0)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("sorcerer;true", "master sorcerer;true")
spell:register()
