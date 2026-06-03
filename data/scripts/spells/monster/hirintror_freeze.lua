local combat = Combat(MonsterCombats.HirintrorFreeze)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("hirintror freeze")
spell:words("###62")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
