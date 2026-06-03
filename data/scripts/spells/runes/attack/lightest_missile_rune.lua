local combat = Combat(RuneAttackCombats.LightestMissileRune)

do
	local level = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level), 20)
	local magic = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel), 20)
	local outputNode = FormulaNode.random(level / 5 + magic * 0.45 + 3, level / 5 + magic * 0.7 + 4)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(7)
spell:name("Lightest Missile Rune")
spell:runeId(23723)
spell:allowFarUse(true)
spell:charges(10)
spell:runeLevel(1)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:runeMagicLevel(0)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
