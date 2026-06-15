local combat = Combat(RuneAttackCombats.PoisonFieldRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(26)
spell:name("Poison Field Rune")
spell:runeId(2285)
spell:allowFarUse(true)
spell:charges(3)
spell:runeLevel(14)
spell:runeMagicLevel(0)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isBlocking(true, false)
spell:register()
