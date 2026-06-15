local combat = Combat(MonsterCombats.IcicleHeal)

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		local master = target:getMaster()
		if not (target:isPlayer() and not master or master and master:isPlayer()) then
			doTargetCombat(0, target, Combat.DamageType.Healing, 400, 600, CONST_ME_NONE)
		end
	end
	return true
end

spell:name("icicle heal")
spell:words("###59")
spell:blockWalls(true)
spell:needLearn(true)
spell:isSelfTarget(true)
spell:register()
