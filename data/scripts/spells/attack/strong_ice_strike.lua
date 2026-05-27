local combat = Combat()
combat:setDamageType(Combat.DamageType.Ice)
combat:setImpactEffect(CONST_ME_ICEATTACK)
combat:setDistanceEffect(CONST_ANI_SMALLICE)

function onGetFormulaValues(player, level, magicLevel)
	local min = (level / 5) + (magicLevel * 2.8) + 16
	local max = (level / 5) + (magicLevel * 4.4) + 28
	return -min, -max
end

combat:setCallback(CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack", "special")
spell:id(152)
spell:name("Strong Ice Strike")
spell:words("exori gran frigo")
spell:level(80)
spell:mana(60)
spell:range(3)
spell:needCasterTargetOrDirection(true)
spell:blockWalls(true)
spell:cooldown(8000)
spell:groupCooldown(2000, 8000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
