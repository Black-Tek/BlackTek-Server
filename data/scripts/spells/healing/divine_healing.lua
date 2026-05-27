local combat = Combat()
combat:setDamageType(Combat.DamageType.Healing)
combat:setImpactEffect(CONST_ME_MAGIC_BLUE)
combat:setAggressive(false)

function onGetFormulaValues(player, level, magicLevel)
	local min = (level / 5) + (magicLevel * 6.9) + 40
	local max = (level / 5) + (magicLevel * 13.2) + 82
	return min, max
end

combat:setCallback(CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(125)
spell:name("Divine Healing")
spell:words("exura san")
spell:level(35)
spell:mana(160)
spell:isPremium(true)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("paladin;true", "royal paladin;true")
spell:register()
