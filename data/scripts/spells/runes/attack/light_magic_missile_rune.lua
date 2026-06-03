local combat = Combat(RuneAttackCombats.LightMagicMissileRune)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 0.4 + 3, level / 5 + magic * 0.8 + 5)
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
spell:name("Light Magic Missile Rune")
spell:runeId(2287)
spell:allowFarUse(true)
spell:charges(10)
spell:runeLevel(15)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:runeMagicLevel(0)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
