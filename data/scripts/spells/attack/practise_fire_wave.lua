local combat = Combat(AttackCombats.PractiseFireWave)

do
	local outputNode = FormulaNode.random(11, 14)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_INSTANT)

function spell.onCastSpell(creature, variant)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(167)
spell:name("Practise Fire Wave")
spell:words("exevo dis flam hur")
spell:level(1)
spell:mana(5)
spell:needDirection(true)
spell:cooldown(4000)
spell:groupCooldown(2000)
spell:vocation("none;true")
spell:register()
