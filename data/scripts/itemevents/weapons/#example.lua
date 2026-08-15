--[[

-- Note: Weapons only need to be registered in Lua if they require a special method not defined in items.toml such as break chance, equip requirements etc...
-- 		For example, a standard crossbow is fully defined in items.toml
-- Note: Some methods may also exist in items.toml. When defined here in Lua, they typically override the items.toml settings but can also cause issues, so best only define them once

-- Register a weapon
ev:register()
ev:id(itemId)
ev:action(action)	-- "removecount", "removecharge", "move"

"removecount"			-- stackable weapons and ammo, remove 1 per attack
"removecharge"			-- weapons with charges, remove 1 per attack
"move"					-- weapon is "throwable" and thrown toward the target on attack, landing on or around their position (one item per attack if stackable)

-- Equip Requirements
ev:level(level)
ev:magicLevel(magicLevel)
ev:premium(bool)
ev:wieldUnproperly(bool) 			 	-- (default: false). Reduces weapon damage when equip requirements are not met
ev:vocation(vocName[, showInDescription, lastVoc])

Vocation requirement string building:
ev:vocation("Knight", true, false)   	-- "knights"
ev:vocation("Paladin", true, false)  	-- "knights, paladins"
ev:vocation("Druid", true, true)     	-- "knights, paladins, and druids" (lasVoc = "true" adds "and" before this vocation)
ev:vocation("Paladin", false, true)  	--  No change because showInDescription = false
Note: "lastVoc" applies only to vocations shown in the description, not the actual last vocation set

-- onUse requirements (deducted per attack)
ev:mana(mana)
ev:manaPercent(percent)
ev:health(health)
ev:healthPercent(percent)
ev:soul(soul)

-- Stats
ev:element(combatType)						-- Default combat type, physical by default
ev:attack(value)
ev:defense(value)
ev:range(tiles)
ev:charges(count)
ev:duration(seconds)
ev:decayTo(itemId)
ev:transformEquipTo(itemId)
ev:transformDeEquipTo(itemId)
ev:slotType(slot) 							-- CONST_SLOT_LEFT, CONST_SLOT_RIGHT...
ev:hitChance(percent)
ev:extraElement(attackValue, combatType)	-- Secondary combat type, additional to the default combat type
ev:breakChance(percent)

-- Distance/Ammo only
ev:ammoType(ammoType)						-- AMMO_ARROW, AMMO_SPEAR etc...
ev:maxHitChance(percent)

-- Wands only
ev:damage(min, max)

-- Wands & Distance weapons
ev:shootType(distEffect)

-- onUseAsWeapon(player, item, var) - unlike the old Weapon's onUseWeapon(player, var), this
-- gains the `item` parameter for consistency with every other ItemEvent hook.

]]--


--Burst Arrow example
local area = createCombatArea({
	{1, 1, 1},
	{1, 3, 1},
	{1, 1, 1}
})

local combat = Combat()
combat:setDamageType(Combat.DamageType.Physical)
combat:setImpactEffect(CONST_ME_EXPLOSIONAREA)
combat:setBlockedByArmor(true)
combat:setFormula(COMBAT_FORMULA_SKILL, 0, 0, 1, 0)
combat:setArea(area)

local burstarrow = ItemEvent(WEAPON_AMMO)

burstarrow.onUseAsWeapon = function(player, item, variant)
	if player:getSkull() == SKULL_BLACK then
		return false
	end

	return combat:execute(player, variant)
end

burstarrow:id(2546)
burstarrow:attack(27)
burstarrow:shootType(CONST_ANI_BURSTARROW)
burstarrow:ammoType("arrow")
burstarrow:maxHitChance(100)
burstarrow:register()


-- Wand of Vortex example
local wov = ItemEvent(WEAPON_WAND)
wov:id(2190)
wov:damage(8, 18)
wov:element("energy")
wov:level(7)
wov:mana(2)
wov:vocation("sorcerer", true, true)
wov:vocation("master sorcerer")
wov:register()

-- Arbalest example
local arbalest = ItemEvent(WEAPON_DISTANCE)
arbalest:id(5803)
arbalest:slotType("two-handed") -- it's now a 2h weapon
arbalest:ammoType("bolt")
arbalest:range(6)
arbalest:attack(2)
arbalest:hitChance(2)
arbalest:level(75)
arbalest:wieldUnproperly(true)
arbalest:register()

-- Earth Barbarian Axe example
local eba = ItemEvent(WEAPON_AXE)
eba:id(7859)
eba:attack(23)
eba:defense(18, 1)
eba:extraElement(5, Combat.DamageType.Earth)
eba:charges(1000, true) -- showCharges = true
eba:action("removecharge")
eba:decayTo(2429)
eba:register()
