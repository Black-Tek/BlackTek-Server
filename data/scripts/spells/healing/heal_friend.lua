local combat = Combat(HealingCombats.HealFriend)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 6.3 + 45, level / 5 + magic * 14.4 + 90)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	if creature:isPlayer() and variant:getNumber() == creature:getId() then
		creature:sendCancelMessage("You can't cast this spell on yourself.")
		creature:getPosition():sendMagicEffect(CONST_ME_POFF)
		return false
	end
	creature:getPosition():sendMagicEffect(CONST_ME_MAGIC_BLUE)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(84)
spell:name("Heal Friend")
spell:words("exura sio")
spell:level(18)
spell:mana(120)
spell:isAggressive(false)
spell:blockWalls(true)
spell:needTarget(true)
spell:hasPlayerNameParam(true)
spell:hasParams(true)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
