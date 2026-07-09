local combat = Combat(PartyCombats.ProtectParty)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return creature:addPartyCondition(combat, variant, PartyCombats.ProtectPartyCondition, 90)
end

spell:group("support")
spell:id(127)
spell:name("Protect Party")
spell:words("utamo mas sio")
spell:level(32)
spell:mana(0)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("paladin;true", "royal paladin;true")
spell:register()
