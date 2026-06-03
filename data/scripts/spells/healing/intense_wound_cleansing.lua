local combat = Combat(HealingCombats.IntenseWoundCleansing)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 70 + 438, level / 5 + magic * 92 + 544)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(158)
spell:name("Intense Wound Cleansing")
spell:words("exura gran ico")
spell:level(80)
spell:mana(200)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:cooldown(600000)
spell:groupCooldown(1000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
