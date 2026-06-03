local combat = Combat(RuneAttackCombats.LightestMagicMissileRune)

do
	local outputNode = FormulaNode.random(3, 7)
	for sit = 0, 3 do
		combat:registerFormula(Combat.FormulaStage.Output, sit, outputNode)
	end
end

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(7)
spell:name("Lightest Magic Missile Rune")
spell:runeId(19392)
spell:allowFarUse(true)
spell:charges(10)
spell:runeLevel(1)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:runeMagicLevel(0)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
