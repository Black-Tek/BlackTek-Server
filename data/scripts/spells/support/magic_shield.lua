local combat = Combat()
combat:setParameter(COMBAT_PARAM_EFFECT, CONST_ME_MAGIC_BLUE)
combat:setParameter(COMBAT_PARAM_AGGRESSIVE, false)

local condition = Condition(CONDITION_MANASHIELD)
condition:setParameter(CONDITION_PARAM_TICKS, 200 * 1000)
combat:addCondition(condition)

local magicShield = Spell(SPELL_INSTANT)

function magicShield.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

magicShield:name("Magic Shield")
magicShield:id(44)
magicShield:words("utamo vita")
magicShield:level(14)
magicShield:mana(50)
magicShield:group("support")
magicShield:isAggressive(false)
magicShield:isSelfTarget(true)
magicShield:cooldown(2000)
magicShield:groupCooldown(2000)
magicShield:needLearn(false)
magicShield:vocation("druid;true", "elder druid;true", "sorcerer;true", "master sorcerer;true")
magicShield:register()
