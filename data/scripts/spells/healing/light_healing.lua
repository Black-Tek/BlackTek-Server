local combat = Combat(HealingCombats.LightHealing)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 1.4 + 8, level / 5 + magic * 1.8 + 11)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(1)
spell:name("Light Healing")
spell:words("exura")
spell:level(8)
spell:mana(20)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("sorcerer;true", "druid;true", "paladin;true", "master sorcerer;true", "elder druid;true", "royal paladin;true")
spell:register()
