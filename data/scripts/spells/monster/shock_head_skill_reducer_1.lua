local combat = Combat(MonsterCombats.ShockHeadSkillReducer1)

local parameters = {{key = CONDITION_PARAM_TICKS, value = 6 * 1000}, {key = CONDITION_PARAM_SKILL_SHIELDPERCENT, value = 65}}

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		target:addAttributeCondition(parameters)
	end
	return true
end

spell:name("shock head skill reducer 1")
spell:words("###43")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
