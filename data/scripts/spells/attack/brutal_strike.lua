local combat = Combat(AttackCombats.BrutalStrike)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local skill = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponSkill)
	local attack = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponAttack)
	local outputNode = FormulaNode.random(level / 5 + skill * attack * 0.02 + 4, level / 5 + skill * attack * 0.04 + 9)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(61)
spell:name("Brutal Strike")
spell:words("exori ico")
spell:level(16)
spell:mana(30)
spell:range(1)
spell:needTarget(true)
spell:needWeapon(true)
spell:cooldown(6000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
