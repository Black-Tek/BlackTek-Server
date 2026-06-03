local combat = Combat(RuneAttackCombats.EnergyWallRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(33)
spell:name("Energy Wall Rune")
spell:runeId(2279)
spell:allowFarUse(true)
spell:charges(4)
spell:runeLevel(41)
spell:runeMagicLevel(9)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isBlocking(true, false)
spell:register()
