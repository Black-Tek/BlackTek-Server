local combat = Combat()
combat:setImpactEffect(CONST_ME_MAGIC_BLUE)
combat:setAggressive(false)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(146)
spell:name("Cure Electrification")
spell:words("exana vis")
spell:level(22)
spell:mana(30)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(6000)
spell:groupCooldown(1000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
