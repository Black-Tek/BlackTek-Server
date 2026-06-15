local combat = Combat(MonsterCombats.HellspawnSoulfire)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_FIRE, DAMAGELIST_VARYING_PERIOD, 10, {8, 10}, 26)
	end
	return true
end

spell:name("hellspawn soulfire")
spell:words("###7")
spell:blockWalls(true)
spell:range(7)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
