local combat = Combat(MonsterCombats.FerumbrasSoulfire)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("ferumbras soulfire")
spell:words("###66")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
