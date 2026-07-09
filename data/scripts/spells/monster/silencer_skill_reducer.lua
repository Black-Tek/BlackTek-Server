local combat = Combat(MonsterCombats.SilencerSkillReducer)

local parameters = {{key = CONDITION_PARAM_TICKS, value = 6 * 1000}, {key = CONDITION_PARAM_STAT_MAGICPOINTSPERCENT, value = nil}}

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	parameters[2].value = math.random(20, 70)

	for _, target in ipairs(combat:getTargets(creature, variant)) do
		target:addAttributeCondition(parameters)
	end
	return true
end

spell:name("silencer skill reducer")
spell:words("###29")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
