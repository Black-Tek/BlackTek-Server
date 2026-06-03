local combat = Combat(MonsterCombats.HealMonsters9x9)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		if not target:isPlayer() and not target:getMaster() then
			doTargetCombatHealth(0, target, Combat.DamageType.Healing, 0, 1000, CONST_ME_NONE)
		end
	end
	return true
end

spell:name("heal monster 9x9")
spell:words("###70")
spell:needDirection(true)
spell:isAggressive(false)
spell:blockWalls(true)
spell:needLearn(true)
spell:register()
