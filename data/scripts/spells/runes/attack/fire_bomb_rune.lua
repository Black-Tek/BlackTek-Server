local combat = Combat(RuneAttackCombats.FireBombRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(17)
spell:name("Fire Bomb Rune")
spell:runeId(2305)
spell:allowFarUse(true)
spell:charges(2)
spell:runeLevel(27)
spell:runeMagicLevel(5)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isBlocking(true, false)
spell:register()
