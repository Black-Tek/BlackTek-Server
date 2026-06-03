local combat = Combat(HealingCombats.MagicPatch)

do
	local level = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level), 20)
	local magic = FormulaNode.min(FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel), 20)
	local outputNode = FormulaNode.random(level / 5 + magic * 0.6 + 3, level / 5 + magic + 6)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(174)
spell:name("Magic Patch")
spell:words("exura infir")
spell:level(1)
spell:mana(6)
spell:isAggressive(false)
spell:isSelfTarget(true)
spell:cooldown(1000)
spell:groupCooldown(1000)
spell:vocation("sorcerer;true", "druid;true", "paladin;true", "master sorcerer;true", "elder druid;true", "royal paladin;true")
spell:register()
