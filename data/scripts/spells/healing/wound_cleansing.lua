local combat = Combat(HealingCombats.WoundCleansing)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 4 + 25, level / 5 + magic * 8 + 50)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(123)
spell:name("Wound Cleansing")
spell:words("exura ico")
spell:level(8)
spell:mana(40)
spell:isPremium(true)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
