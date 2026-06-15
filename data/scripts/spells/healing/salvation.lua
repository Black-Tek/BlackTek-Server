local combat = Combat(HealingCombats.Salvation)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 12 + 75, level / 5 + magic * 20 + 125)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(36)
spell:name("Salvation")
spell:words("exura gran san")
spell:level(60)
spell:mana(210)
spell:isSelfTarget(true)
spell:isAggressive(false)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("paladin;true", "royal paladin;true")
spell:register()
