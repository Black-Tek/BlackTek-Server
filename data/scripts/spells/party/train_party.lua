local combat = Combat(PartyCombats.TrainParty)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return creature:addPartyCondition(combat, variant, PartyCombats.TrainPartyCondition, 60)
end

spell:group("support")
spell:id(126)
spell:name("Train Party")
spell:words("utito mas sio")
spell:level(32)
spell:mana(0)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
