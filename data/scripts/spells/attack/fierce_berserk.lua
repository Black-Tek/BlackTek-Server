local combat = Combat(AttackCombats.FierceBerserk)

do
	local level = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.Level)
	local skill = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponSkill)
	local attack = FormulaNode.bind(Combat.BindSource.Caster, Combat.BindKey.WeaponAttack)
	local outputNode = FormulaNode.random(level / 5 + skill * attack * 0.06 + 13, level / 5 + skill * attack * 0.11 + 27)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(105)
spell:name("Fierce Berserk")
spell:words("exori gran")
spell:level(90)
spell:mana(340)
spell:needWeapon(true)
spell:cooldown(6000)
spell:groupCooldown(2000)
spell:vocation("knight;true", "elite knight;true")
spell:register()
