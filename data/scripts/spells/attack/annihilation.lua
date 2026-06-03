local combat = Combat(AttackCombats.Annihilation)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local skill = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponSkill)
	local attack = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponAttack)
	local outputNode = FormulaNode.random(level / 5 + skill * attack * 0.06 + 13, level / 5 + skill * attack * 0.14 + 34)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(62)
spell:name("Annihilation")
spell:words("exori gran ico")
spell:level(110)
spell:mana(300)
spell:range(1)
spell:needTarget(true)
spell:needWeapon(true)
spell:cooldown(30000)
spell:groupCooldown(4000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
