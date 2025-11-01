--[[

-- Note: Weapons only need to be registered in Lua if they require a special method not defined in items.toml such as break chance, equip requirements etc...
-- 		For example, a standard crossbow is fully defined in items.toml
-- Note: Some methods may also exist in items.toml. When defined here in Lua, they typically override the items.toml settings but can also cause issues, so best only define them once

-- Register a weapon
weapon:register()
weapon:id(itemId)
weapon:action(action)	-- "removecount", "removecharge", "move"

"removecount"			-- stackable weapons and ammo, remove 1 per attack
"removecharge"			-- weapons with charges, remove 1 per attack
"move"					-- weapon is "throwable" and thrown toward the target on attack, landing on or around their position (one item per attack if stackable)

-- Equip Requirements
weapon:level(level)
weapon:magicLevel(magicLevel)
weapon:premium(bool)
weapon:wieldUnproperly(bool) 			 	-- (default: false). Reduces weapon damage when equip requirements are not met
weapon:vocation(vocName[, showInDescription, lastVoc])

Vocation requirement string building:
weapon:vocation("Knight", true, false)   	-- "knights"
weapon:vocation("Paladin", true, false)  	-- "knights, paladins"
weapon:vocation("Druid", true, true)     	-- "knights, paladins, and druids" (lasVoc = "true" adds "and" before this vocation)
weapon:vocation("Paladin", false, true)  	--  No change because showInDescription = false
Note: "lastVoc" applies only to vocations shown in the description, not the actual last vocation set

-- onUse requirements (deducted per attack)
weapon:mana(mana)
weapon:manaPercent(percent)
weapon:health(health)
weapon:healthPercent(percent)
weapon:soul(soul)

-- Stats
weapon:element(combatType)						-- Default combat type, physical by default
weapon:attack(value)
weapon:defense(value)
weapon:range(tiles)
weapon:charges(count)
weapon:duration(seconds)
weapon:decayTo(itemId)
weapon:transformEquipTo(itemId)
weapon:transformDeEquipTo(itemId)
weapon:slotType(slot) 							-- CONST_SLOT_LEFT, CONST_SLOT_RIGHT...
weapon:hitChance(percent)
weapon:extraElement(attackValue, combatType)	-- Secondary combat type, additional to the default combat type
weapon:breakChance(percent)

-- Distance/Ammo only
weapon:ammoType(ammoType)						-- AMMO_ARROW, AMMO_SPEAR etc...
weapon:maxHitChance(percent)

-- Wands only
weapon:damage(min, max)

-- Wands & Distance weapons
weapon:shootType(distEffect)

]]--


--Burst Arrow example
local area = createCombatArea({
	{1, 1, 1},
	{1, 3, 1},
	{1, 1, 1}
})

local combat = Combat()
combat:setParameter(COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE)
combat:setParameter(COMBAT_PARAM_EFFECT, CONST_ME_EXPLOSIONAREA)
combat:setParameter(COMBAT_PARAM_BLOCKARMOR, true)
combat:setFormula(COMBAT_FORMULA_SKILL, 0, 0, 1, 0)
combat:setArea(area)

local burstarrow = Weapon(WEAPON_AMMO)

burstarrow.onUseWeapon = function(player, variant)
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
local wov = Weapon(WEAPON_WAND)
wov:id(2190)
wov:damage(8, 18)
wov:element("energy")
wov:level(7)
wov:mana(2)
wov:vocation("sorcerer", true, true)
wov:vocation("master sorcerer")
wov:register()

-- Arbalest example
local arbalest = Weapon(WEAPON_DISTANCE)
arbalest:id(5803)
arbalest:slotType("two-handed") -- it's now a 2h weapon
arbalest:ammoType("bolt")
arbalest:range(6)
arbalest:attack(2)
arbalest:hitChance(2)
arbalest:level(75)
arbalest:wieldedUnproperly(true)
arbalest:register()

-- Earth Barbarian Axe example
local eba = Weapon(WEAPON_AXE)
eba:id(7859)
eba:attack(23)
eba:defense(18, 1)
eba:extraElement(5, COMBAT_EARTHDAMAGE)
eba:charges(1000, true) -- showCharges = true
eba:action("removecharge")
eba:decayTo(2429)
eba:register()
