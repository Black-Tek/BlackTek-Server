local combat = Combat(MonsterCombats.ExplosionWave)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("explosion wave")
spell:words("###54")
spell:needDirection(true)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
