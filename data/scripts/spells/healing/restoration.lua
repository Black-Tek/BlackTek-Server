local combat = Combat(HealingCombats.Restoration)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local minBase = level / 5 + magic * 6.8 + 42
	local maxBase = level / 5 + magic * 12.9 + 90
	local outputNode = FormulaNode.random(minBase * 1.4, maxBase * 1.4)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(241)
spell:name("Restoration")
spell:words("exura max vita")
spell:level(300)
spell:mana(260)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(6000)
spell:groupCooldown(1000)
spell:vocation("sorcerer;true", "druid;true", "master sorcerer;true", "elder druid;true")
spell:register()
