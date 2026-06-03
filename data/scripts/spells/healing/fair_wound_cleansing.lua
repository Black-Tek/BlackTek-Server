local combat = Combat(HealingCombats.FairWoundCleansing)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local minBase = level / 5 + magic * 4 + 25
	local maxBase = level / 5 + magic * 8 + 50
	local outputNode = FormulaNode.random(minBase * 2, maxBase * 2)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(239)
spell:name("Fair Wound Cleansing")
spell:words("exura med ico")
spell:level(300)
spell:mana(90)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
