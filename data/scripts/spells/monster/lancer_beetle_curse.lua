local combat = Combat(MonsterCombats.LancerBeetleCurse)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	local damage = math.random(17, 43)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_CURSED, DAMAGELIST_EXPONENTIAL_DAMAGE, damage)
	end
	return true
end

spell:name("lancer beetle curse")
spell:words("###12")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
