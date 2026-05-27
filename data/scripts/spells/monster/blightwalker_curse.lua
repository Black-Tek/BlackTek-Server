local combat = Combat()
combat:setDamageType(Combat.DamageType.Death)
combat:setImpactEffect(CONST_ME_SMALLCLOUDS)
combat:setArea(createCombatArea(AREA_CIRCLE6X6))

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	local damage = math.random(52, 154)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_CURSED, DAMAGELIST_EXPONENTIAL_DAMAGE, damage)
	end
	return true
end

spell:name("blightwalker curse")
spell:words("###16")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
