local combat = Combat(MonsterCombats.EnergyPulseExplosion)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant), creature:remove()
end

spell:name("energy pulse explosion")
spell:words("###64")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
