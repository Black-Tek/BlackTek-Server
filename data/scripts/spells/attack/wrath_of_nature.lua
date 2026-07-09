local combat = Combat(AttackCombats.WrathOfNature)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 3 + 32, level / 5 + magic * 9 + 40)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(56)
spell:name("Wrath of Nature")
spell:words("exevo gran mas tera")
spell:level(55)
spell:mana(700)
spell:isSelfTarget(true)
spell:cooldown(40000)
spell:groupCooldown(4000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
