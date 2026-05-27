local combat = Combat()
combat:setDamageType(Combat.DamageType.Death)
combat:setImpactEffect(CONST_ME_SMALLCLOUDS)
combat:setDistanceEffect(CONST_ANI_DEATH)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	local damage = math.random(74, 107)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_CURSED, DAMAGELIST_EXPONENTIAL_DAMAGE, damage)
	end
	return true
end

spell:name("lizard magistratus curse")
spell:words("###13")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
