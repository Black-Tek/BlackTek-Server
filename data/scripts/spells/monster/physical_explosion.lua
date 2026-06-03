local combat = Combat(MonsterCombats.PhysicalExplosion)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("physical explosion")
spell:words("###68")
spell:isAggressive(false)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
