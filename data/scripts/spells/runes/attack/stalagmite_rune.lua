local combat = Combat(RuneAttackCombats.StalagmiteRune)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 0.8 + 5, level / 5 + magic * 1.6 + 10)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(77)
spell:name("Stalagmite Rune")
spell:runeId(2292)
spell:allowFarUse(true)
spell:charges(10)
spell:runeLevel(24)
spell:runeMagicLevel(3)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:needTarget(true)
spell:register()
