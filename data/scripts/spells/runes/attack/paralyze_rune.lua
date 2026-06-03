local combat = Combat(RuneAttackCombats.ParalyzeRune)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	if not combat:execute(creature, variant) then
		return false
	end

	creature:getPosition():sendMagicEffect(CONST_ME_MAGIC_GREEN)
	return true
end

spell:group("attack")
spell:id(54)
spell:name("Paralyze Rune")
spell:runeId(2278)
spell:allowFarUse(true)
spell:charges(1)
spell:runeLevel(54)
spell:runeMagicLevel(18)
spell:isPzLock(true)
spell:cooldown(6000)
spell:groupCooldown(6000)
spell:mana(1400)
spell:needTarget(true)
spell:isBlocking(true, false)
spell:vocation("druid;true", "elder druid")
spell:register()
