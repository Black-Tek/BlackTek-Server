local combat = Combat(AttackCombats.StrongIceWave)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 4.5 + 20, level / 5 + magic * 7.6 + 48)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(43)
spell:name("Strong Ice Wave")
spell:words("exevo gran frigo hur")
spell:level(40)
spell:mana(170)
spell:needDirection(true)
spell:cooldown(8000)
spell:groupCooldown(2000)
spell:vocation("druid;true", "elder druid;true")
spell:register()
