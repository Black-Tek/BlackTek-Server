local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setImpactEffect(CONST_ME_HITAREA)
combat:setBlockedByArmor(true)
combat:setUseCharges(true)
combat:setArea(createCombatArea(AREA_WAVE6, AREADIAGONAL_WAVE6))

function onGetFormulaValues(player, skill, attack, factor)
	local min = (player:getLevel() / 5) + (skill * attack * 0.04) + 11
	local max = (player:getLevel() / 5) + (skill * attack * 0.08) + 21
	return -min, -max
end

combat:setCallback(CALLBACK_PARAM_SKILLVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(59)
spell:name("Front Sweep")
spell:words("exori min")
spell:level(70)
spell:mana(200)
spell:needWeapon(true)
spell:needDirection(true)
spell:cooldown(6000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
