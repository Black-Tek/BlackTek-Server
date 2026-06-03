local combat = Combat(SupportCombats.Challenge)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		doChallengeCreature(creature, target)
	end
	return true
end

spell:group("support")
spell:id(93)
spell:name("Challenge")
spell:words("exeta res")
spell:level(20)
spell:mana(40)
spell:isAggressive(false)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:vocation("elite knight;true")
spell:register()
