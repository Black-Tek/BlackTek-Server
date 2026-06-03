local combat = Combat(MonsterCombats.WarGolemSkillReducer)

local parameters = {{key = CONDITION_PARAM_TICKS, value = 3 * 1000}, {key = CONDITION_PARAM_SKILL_SHIELDPERCENT, value = 70}}

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		target:addAttributeCondition(parameters)
	end
	return true
end

spell:name("war golem skill reducer")
spell:words("###28")
spell:needDirection(true)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
