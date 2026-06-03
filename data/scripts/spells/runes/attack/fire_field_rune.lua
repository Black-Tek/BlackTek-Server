local combat = Combat(RuneAttackCombats.FireFieldRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(25)
spell:name("Fire Field Rune")
spell:runeId(2301)
spell:allowFarUse(true)
spell:charges(3)
spell:runeLevel(15)
spell:runeMagicLevel(1)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isBlocking(true, false)
spell:register()
