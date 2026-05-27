local combat = Combat()
combat:setDamageType(Combat.DamageType.Energy)
combat:setImpactEffect(CONST_ME_ENERGYAREA)
combat:setDistanceEffect(CONST_ANI_ENERGY)

function onGetFormulaValues(player, level, magicLevel)
	local min = (level / 5) + (magicLevel * 2.2) + 12
	local max = (level / 5) + (magicLevel * 3.4) + 21
	return -min, -max
end

combat:setCallback(CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack", "special")
spell:id(149)
spell:name("Lightning")
spell:words("exori amp vis")
spell:level(55)
spell:mana(60)
spell:range(5)
spell:needCasterTargetOrDirection(true)
spell:blockWalls(true)
spell:cooldown(8000)
spell:groupCooldown(2000, 8000)
spell:vocation("sorcerer;true", "master sorcerer;true")
spell:register()
