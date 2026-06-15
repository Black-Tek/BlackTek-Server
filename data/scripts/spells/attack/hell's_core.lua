local combat = Combat(AttackCombats.HellsCore)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 8 + 50, level / 5 + magic * 12 + 75)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(24)
spell:name("Hell's Core")
spell:words("exevo gran mas flam")
spell:level(60)
spell:mana(1100)
spell:cooldown(40000)
spell:groupCooldown(4000)
spell:isSelfTarget(true)
spell:vocation("sorcerer;true", "master sorcerer;true")
spell:register()
