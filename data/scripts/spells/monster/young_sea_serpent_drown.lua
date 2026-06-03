local combat = Combat(MonsterCombats.YoungSeaSerpentDrown)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_DROWN, DAMAGELIST_CONSTANT_PERIOD, 20, 5, 6)
	end
	return true
end

spell:name("young sea serpent drown")
spell:words("###18")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
