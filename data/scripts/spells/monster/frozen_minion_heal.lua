local combat = Combat(MonsterCombats.FrozenMinionHeal)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		local master = target:getMaster()
		if not (target:isPlayer() and not master or master and master:isPlayer()) then
			doTargetCombat(0, target, Combat.DamageType.Healing, 100, 200, CONST_ME_NONE)
		end
	end
	return true
end

spell:name("frozen minion heal")
spell:words("###57")
spell:blockWalls(true)
spell:needLearn(true)
spell:isSelfTarget(true)
spell:register()
