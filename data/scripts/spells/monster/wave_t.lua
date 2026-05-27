local area = {{1, 1, 1}, {0, 1, 0}, {0, 3, 0}}

local combat = Combat()
combat:setDamageType(Combat.DamageType.Earth)
combat:setImpactEffect(CONST_ME_GREEN_RINGS)
combat:setArea(createCombatArea(area))

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
