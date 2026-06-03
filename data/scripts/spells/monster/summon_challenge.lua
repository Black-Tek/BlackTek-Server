local combat = Combat(MonsterCombats.SummonChallenge)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		doChallengeCreature(creature, target)
	end
	return true
end

spell:name("summon challenge")
spell:words("###52")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
