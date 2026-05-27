local combat = Combat()
combat:setDamageType(Combat.DamageType.Healing)
combat:setImpactEffect(CONST_ME_MAGIC_BLUE)
combat:setAggressive(false)

function onGetFormulaValues(player, level, magicLevel)
	return 5, 9
end

combat:setCallback(CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(166)
spell:name("Practise Healing")
spell:words("exura dis")
spell:level(1)
spell:mana(5)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("none;true")
spell:register()
