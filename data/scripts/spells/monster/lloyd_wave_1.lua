local combat = Combat(MonsterCombats.LloydWave1)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("lloyd wave 1")
spell:words("###60")
spell:blockWalls(true)
spell:needLearn(true)
spell:needDirection(true)
spell:register()
