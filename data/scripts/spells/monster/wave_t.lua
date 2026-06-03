local combat = Combat(MonsterCombats.WaveT)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("wave t")
spell:words("###55")
spell:needDirection(true)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
