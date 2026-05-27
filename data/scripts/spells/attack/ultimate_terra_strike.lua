local combat = Combat()
combat:setDamageType(Combat.DamageType.Earth)
combat:setImpactEffect(CONST_ME_CARNIPHILA)
combat:setDistanceEffect(CONST_ANI_SMALLEARTH)

function onGetFormulaValues(player, level, magicLevel)
	local min = (level / 5) + (magicLevel * 4.5) + 35
	local max = (level / 5) + (magicLevel * 7.3) + 55
	return -min, -max
end

combat:setCallback(CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(157)
spell:name("Ultimate Terra Strike")
spell:words("exori max tera")
spell:level(90)
spell:mana(100)
spell:range(3)
spell:needCasterTargetOrDirection(true)
spell:blockWalls(true)
spell:cooldown(30000)
spell:groupCooldown(4000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
