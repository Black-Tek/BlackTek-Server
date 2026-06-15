local combat = Combat(MonsterCombats.PhanstamDrown)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_DROWN, DAMAGELIST_CONSTANT_PERIOD, 20, 5, 20)
	end
	return true
end

spell:name("phantasm drown")
spell:words("###9")
spell:needDirection(true)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
