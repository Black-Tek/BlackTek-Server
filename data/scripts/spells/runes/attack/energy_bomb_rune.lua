local combat = Combat(RuneAttackCombats.EnergyBombRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(55)
spell:name("Energy Bomb Rune")
spell:runeId(2262)
spell:allowFarUse(true)
spell:charges(2)
spell:runeLevel(37)
spell:runeMagicLevel(10)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isBlocking(true, false)
spell:register()
