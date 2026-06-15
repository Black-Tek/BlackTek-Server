local combat = Combat(AttackCombats.RageOfTheSkies)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local magic = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.MagicLevel)
	local outputNode = FormulaNode.random(level / 5 + magic * 4 + 75, level / 5 + magic * 10 + 150)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(119)
spell:name("Rage of the Skies")
spell:words("exevo gran mas vis")
spell:level(55)
spell:mana(600)
spell:isSelfTarget(true)
spell:cooldown(40000)
spell:groupCooldown(4000)
spell:vocation("sorcerer;true", "master sorcerer;true")
spell:register()
