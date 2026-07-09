local combat = Combat(AttackCombats.Scorch)

do
	local level = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level), 20)
	local magic = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel), 20)
	local outputNode = FormulaNode.random(level / 5 + magic * 0.3 + 2, level / 5 + magic * 0.45 + 3)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(178)
spell:name("Scorch")
spell:words("exevo infir flam hur")
spell:level(1)
spell:mana(8)
spell:needDirection(true)
spell:cooldown(4000)
spell:groupCooldown(2000)
spell:vocation("sorcerer;true", "master sorcerer;true")
spell:register()
