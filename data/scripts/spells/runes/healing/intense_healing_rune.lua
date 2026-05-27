local combat = Combat()
combat:setDamageType(Combat.DamageType.Healing)
combat:setImpactEffect(CONST_ME_MAGIC_BLUE)
combat:setTopTargetOnly(true)
combat:setAggressive(false)

function onGetFormulaValues(player, level, magicLevel)
	local min = (level / 5) + (magicLevel * 3.2) + 20
	local max = (level / 5) + (magicLevel * 5.4) + 40
	return min, max
end

combat:setCallback(CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(4)
spell:name("Intense Healing Rune")
spell:runeId(2265)
spell:allowFarUse(true)
spell:charges(1)
spell:runeLevel(15)
spell:runeMagicLevel(1)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isAggressive(false)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
