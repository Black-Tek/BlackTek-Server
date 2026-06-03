local combat = Combat(MonsterCombats.PixieSkillReducer)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("pixie skill reducer")
spell:words("###72")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
