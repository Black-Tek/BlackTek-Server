local combat = Combat(MonsterCombats.MagmaCrawlerSoulfire)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		creature:addDamageCondition(target, CONDITION_FIRE, DAMAGELIST_VARYING_PERIOD, 10, {8, 10}, 20)
	end
	return true
end

spell:name("magma crawler soulfire")
spell:words("###27")
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
