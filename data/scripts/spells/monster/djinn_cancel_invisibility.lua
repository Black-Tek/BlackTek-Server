local combat = Combat()
combat:setArea(createCombatArea(AREA_CIRCLE3X3))

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:name("djinn cancel invisibility")
spell:words("###47")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
