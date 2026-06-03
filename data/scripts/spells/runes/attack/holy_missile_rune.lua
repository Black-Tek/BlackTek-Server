local combat = Combat(RuneAttackCombats.HolyMissileRune)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 1.8 + 11, level / 5 + magic * 3.8 + 23)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(130)
spell:name("Holy Missile Rune")
spell:runeId(2295)
spell:allowFarUse(true)
spell:charges(5)
spell:runeLevel(27)
spell:runeMagicLevel(4)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:vocation("paladin;true", "royal paladin")
spell:register()
