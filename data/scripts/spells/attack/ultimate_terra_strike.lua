local combat = Combat(AttackCombats.UltimateTerraStrike)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 4.5 + 35, level / 5 + magic * 7.3 + 55)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(157)
spell:name("Ultimate Terra Strike")
spell:words("exori max tera")
spell:level(90)
spell:mana(100)
spell:range(3)
spell:needCasterTargetOrDirection(true)
spell:blockWalls(true)
spell:cooldown(30000)
spell:groupCooldown(4000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
