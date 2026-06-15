local combat = Combat(AttackCombats.StrongEtherealSpear)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local dist = FormulaNode.bindSkill(Combat.BindSource.Caster, SKILL_DISTANCE)
	local outputNode = FormulaNode.random(level / 5 + dist + 7, level / 5 + dist * 1.5 + 13)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(57)
spell:name("Strong Ethereal Spear")
spell:words("exori gran con")
spell:level(90)
spell:mana(55)
spell:range(7)
spell:needTarget(true)
spell:cooldown(8000)
spell:groupCooldown(2000)
spell:blockWalls(true)
spell:vocation("paladin;true", "royal paladin;true")
spell:register()
