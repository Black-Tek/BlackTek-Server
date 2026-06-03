local combat = Combat(MonsterCombats.SouleaterDrown)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_DROWN, DAMAGELIST_CONSTANT_PERIOD, 20, 5, 10)
	end
	return true
end

spell:name("souleater drown")
spell:words("###23")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
