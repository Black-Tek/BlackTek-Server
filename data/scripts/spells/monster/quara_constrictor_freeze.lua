local combat = Combat(MonsterCombats.QuaraConstrictorFreeze)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_FREEZING, DAMAGELIST_VARYING_PERIOD, 8, {8, 10}, 10)
	end
	return true
end

spell:name("quara constrictor freeze")
spell:words("###19")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
