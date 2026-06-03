local combat = Combat(RuneAttackCombats.SuddenDeathRune)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 4.3 + 32, level / 5 + magic * 7.4 + 48)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(21)
spell:name("Sudden Death Rune")
spell:runeId(2268)
spell:allowFarUse(true)
spell:charges(3)
spell:runeLevel(45)
spell:runeMagicLevel(15)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
