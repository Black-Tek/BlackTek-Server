local combat = Combat(RuneHealingCombats.UltimateHealingRune)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 7.3 + 42, level / 5 + magic * 12.4 + 90)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(5)
spell:name("Ultimate Healing Rune")
spell:runeId(2273)
spell:allowFarUse(true)
spell:charges(1)
spell:runeLevel(24)
spell:runeMagicLevel(4)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isAggressive(false)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
