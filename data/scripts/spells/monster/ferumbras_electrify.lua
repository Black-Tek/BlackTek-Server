local combat = Combat(MonsterCombats.FerumbrasElectrify)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("ferumbras electrify")
spell:words("###65")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
