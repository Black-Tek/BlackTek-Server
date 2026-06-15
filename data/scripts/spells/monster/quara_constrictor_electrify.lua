local combat = Combat(MonsterCombats.QuaraConstrictorElectrify)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_ENERGY, DAMAGELIST_VARYING_PERIOD, 25, {10, 12}, 3)
	end
	return true
end

spell:name("quara constrictor electrify")
spell:words("###20")
spell:blockWalls(true)
spell:needTarget(true)
spell:needLearn(true)
spell:register()
