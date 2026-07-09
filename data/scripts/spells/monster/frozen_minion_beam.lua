local combat = Combat(MonsterCombats.FrozenMinionBeam)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		local master = target:getMaster()
		if target:isPlayer() and not master or master and master:isPlayer() then
			doTargetCombat(0, target, Combat.DamageType.Ice, 200, 700, CONST_ME_NONE)
		else
			doTargetCombat(0, target, Combat.DamageType.Healing, 200, 700, CONST_ME_NONE)
		end
	end
	return true
end

spell:name("frozen minion beam")
spell:words("###58")
spell:blockWalls(true)
spell:needLearn(true)
spell:needDirection(true)
spell:register()
