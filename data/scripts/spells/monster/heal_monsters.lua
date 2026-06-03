local combat = Combat(MonsterCombats.HealMonsters)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		if not target:isPlayer() and not target:getMaster() then
			doTargetCombatHealth(0, target, Combat.DamageType.Healing, 100, 300, CONST_ME_NONE)
		end
	end
	return true
end

spell:name("heal monster")
spell:words("###71")
spell:needDirection(true)
spell:isAggressive(false)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
