local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setImpactEffect(CONST_ME_HITAREA)
combat:setDistanceEffect(CONST_ANI_ETHEREALSPEAR)
combat:setBlockedByArmor(true)

function onGetFormulaValues(player, skill, attack, factor)
	local distanceSkill = player:getEffectiveSkillLevel(SKILL_DISTANCE)
	local min = (player:getLevel() / 5) + distanceSkill + 7
	local max = (player:getLevel() / 5) + (distanceSkill * 1.5) + 13
	return -min, -max
end

combat:setCallback(CALLBACK_PARAM_SKILLVALUE, "onGetFormulaValues")

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(57)
spell:name("Strong Ethereal Spear")
spell:words("exori gran con")
spell:level(90)
spell:mana(55)
spell:range(7)
spell:needTarget(true)
spell:cooldown(8000)
spell:groupCooldown(2000)
spell:blockWalls(true)
spell:vocation("paladin;true", "royal paladin;true")
spell:register()
