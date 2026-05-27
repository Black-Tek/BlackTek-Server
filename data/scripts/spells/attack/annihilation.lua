local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setImpactEffect(CONST_ME_HITAREA)
combat:setDistanceEffect(CONST_ANI_WEAPONTYPE)
combat:setBlockedByArmor(true)
combat:setUseCharges(true)

function onGetFormulaValues(player, skill, attack, factor)
	local min = (player:getLevel() / 5) + (skill * attack * 0.06) + 13
	local max = (player:getLevel() / 5) + (skill * attack * 0.14) + 34
	return -min, -max
end

combat:setCallback(CALLBACK_PARAM_SKILLVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(62)
spell:name("Annihilation")
spell:words("exori gran ico")
spell:level(110)
spell:mana(300)
spell:range(1)
spell:needTarget(true)
spell:needWeapon(true)
spell:cooldown(30000)
spell:groupCooldown(4000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
