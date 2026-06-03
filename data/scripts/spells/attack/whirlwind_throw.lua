local combat = Combat(AttackCombats.WhirlwindThrow)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local skill = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponSkill)
	local attack = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponAttack)
	local outputNode = FormulaNode.random(level / 5 + skill * attack * 0.01 + 1, level / 5 + skill * attack * 0.03 + 6)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(107)
spell:name("Whirlwind Throw")
spell:words("exori hur")
spell:level(28)
spell:mana(40)
spell:range(5)
spell:needTarget(true)
spell:blockWalls(true)
spell:needWeapon(true)
spell:cooldown(6000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
