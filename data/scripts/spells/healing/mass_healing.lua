local combat = Combat()
combat:setImpactEffect(CONST_ME_MAGIC_BLUE)
combat:setAggressive(false)
combat:setArea(createCombatArea(AREA_CIRCLE3X3))

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	local min = (creature:getLevel() / 5) + (creature:getMagicLevel() * 4.6) + 100
	local max = (creature:getLevel() / 5) + (creature:getMagicLevel() * 9.6) + 125
	for _, target in ipairs(combat:getTargets(creature, variant)) do
		local master = target:getMaster()
		if target:isPlayer() or master and master:isPlayer() then
			doTargetCombat(creature, target, Combat.DamageType.Healing, min, max)
		end
	end
	return true
end

spell:group("healing")
spell:id(82)
spell:name("Mass Healing")
spell:words("exura gran mas res")
spell:level(36)
spell:mana(150)
spell:isAggressive(false)
spell:cooldown(2000)
spell:groupCooldown(1000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
