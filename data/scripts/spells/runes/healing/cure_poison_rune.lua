local combat = Combat(RuneHealingCombats.CurePoisonRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("healing")
spell:id(31)
spell:name("Cure Poison Rune")
spell:runeId(2266)
spell:allowFarUse(true)
spell:charges(1)
spell:runeLevel(15)
spell:runeMagicLevel(0)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isAggressive(false)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:register()
