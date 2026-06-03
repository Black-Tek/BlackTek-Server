local combat = Combat(MonsterCombats.LloydWave2)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("lloyd wave 2")
spell:words("###61")
spell:blockWalls(true)
spell:needLearn(true)
spell:needDirection(true)
spell:register()
