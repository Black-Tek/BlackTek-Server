local combat = Combat(MonsterCombats.MassiveEnergyElementalElectrify)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_ENERGY, DAMAGELIST_VARYING_PERIOD, 25, {10, 12}, 10)
	end
	return true
end

spell:name("massive energy elemental electrify")
spell:words("###4")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
