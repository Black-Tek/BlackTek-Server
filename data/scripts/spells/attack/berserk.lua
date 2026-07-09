local combat = Combat(AttackCombats.Berserk)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local skill = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponSkill)
	local attack = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponAttack)
	local outputNode = FormulaNode.random(level / 5 + skill * attack * 0.03 + 7, level / 5 + skill * attack * 0.05 + 11)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(80)
spell:name("Berserk")
spell:words("exori")
spell:level(35)
spell:mana(115)
spell:needWeapon(true)
spell:cooldown(4000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
