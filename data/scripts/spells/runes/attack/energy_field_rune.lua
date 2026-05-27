local combat = Combat()
combat:setDamageType(Combat.DamageType.Energy)
combat:setImpactEffect(CONST_ME_ENERGYHIT)
combat:setDistanceEffect(CONST_ANI_ENERGYBALL)
combat:setCreatedItem(ITEM_ENERGYFIELD_PVP)

local spell = Spell(SPELL_RUNE)

function spell.onCastSpell(creature, variant, isHotkey)
	return combat:execute(creature, variant)
end

spell:group("attack")
spell:id(27)
spell:name("Energy Field Rune")
spell:runeId(2277)
spell:allowFarUse(true)
spell:charges(3)
spell:runeLevel(18)
spell:runeMagicLevel(3)
spell:cooldown(2000)
spell:groupCooldown(2000)
spell:isBlocking(true, false)
spell:register()
