local combat = Combat(RuneHealingCombats.IntenseHealingRune)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 3.2 + 20, level / 5 + magic * 5.4 + 40)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(4)
spell:name("Intense Healing Rune")
spell:runeId(2265)
spell:allowFarUse(true)
spell:charges(1)
spell:runeLevel(15)
spell:runeMagicLevel(1)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isAggressive(false)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
