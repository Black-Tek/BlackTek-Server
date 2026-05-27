local area = {{1, 1, 1}, {1, 1, 1}, {0, 1, 0}, {0, 3, 0}}

local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setImpactEffect(CONST_ME_EXPLOSIONHIT)
combat:setArea(createCombatArea(area))

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
