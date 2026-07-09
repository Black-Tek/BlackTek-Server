local combat = Combat(MonsterCombats.MassiveFireElementalSoulfire)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_FIRE, DAMAGELIST_VARYING_PERIOD, 10, {8, 10}, 20)
	end
	return true
end

spell:name("massive fire elemental soulfire")
spell:words("###5")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
