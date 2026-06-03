local combat = Combat(RuneAttackCombats.LightStoneShowerRune)

do
	local level = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level), 20)
	local magic = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel), 20)
	local outputNode = FormulaNode.random(level / 5 + magic * 0.3 + 2, level / 5 + magic * 0.45 + 3)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(116)
spell:name("Light Stone Shower Rune")
spell:runeId(23722)
spell:allowFarUse(true)
spell:charges(4)
spell:runeLevel(1)
spell:runeMagicLevel(0)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:register()
