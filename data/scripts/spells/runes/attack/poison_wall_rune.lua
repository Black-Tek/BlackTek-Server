local combat = Combat(RuneAttackCombats.PoisonWallRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(32)
spell:name("Poison Wall Rune")
spell:runeId(2289)
spell:allowFarUse(true)
spell:charges(4)
spell:runeLevel(29)
spell:runeMagicLevel(5)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isBlocking(true, false)
spell:register()
