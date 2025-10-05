local mType = Game.createMonsterType("Example Monster")
local monster = {}

monster.description = "an example monster"
monster.experience = 1000

monster.outfit = {
	lookType = 37, 			-- Outfit ID
	-- lookTypeEx = 1551, 	-- Item ID as outfit

	-- Optional values won't be visible unless the outfit supports them
	lookHead = 0, 	-- Optional: Colour (0-132)
	lookBody = 0,
	lookLegs = 0,
	lookFeet = 0,
	lookAddons = 0, -- Optional: Addons (0-3)
	lookMount = 0	-- Optional: Mount ID
}

monster.health = 10000    -- Current health on spawn
monster.maxHealth = 10000 -- Maximum health (should = health in normal cases)
monster.runHealth = 1000  -- Health threshold to flee (0 = never flees)

monster.speed = 200       -- Base speed
monster.manaCost = 0      -- Mana cost for summoning this monster
monster.maxSummons = 10   -- Max creatures this monster can summon when he has a summon spell (0 = no limit but still limited by max per summon)
monster.race = "blood"    -- Race: blood, venom, fire, energy, undead
monster.corpse = 2809     -- Corpse (item id)
monster.skull = ""        -- Optional: "green", "white", "yellow", "orange", "red", "black"

monster.flags = {
	summonable = false,    		-- Can players summon this monster (with summon spell)
	attackable = true,     		-- Can players attack this monster
	hostile = true,        		-- Is the monster aggressive? (false for Deer, Rabbit, Pig etc, true for Dog, Troll, Demon etc)
	convinceable = false,  		-- Can players convince
	boss = false,          		-- Is this a boss?
	challengeable = false, 		-- Respects "exeta res"/challenge type spells
	illusionable = false,  		-- Can be used for illusion spell
	ignoreSpawnBlock = false, 	-- Ignores spawn blocking from players
	pushable = false,      		-- Can be pushed by players/monsters
	canPushItems = true,   		-- Can push items
	canPushCreatures = true,	-- Can push other creatures (disables pushable)
	canWalkOnEnergy = false, 	-- Can walk on energy fields
	canWalkOnFire = false, 		-- Can walk on fire fields
	canWalkOnPoison = false,	-- Can walk on poison fields
	healthHidden = false		-- Hide health bar from players
}

monster.targetDistance = 1      -- Preferred distance from target (1 = melee, 2+ = ranged)
monster.staticAttackChance = 90 -- Chance to not move while attacking the target (lower = more movement = "dancing").

monster.changeTarget = {
	interval = 4000,	-- How often to consider switching target (1000 = 1 second)
	chance = 20			-- % chance to switch per interval
}

monster.light = {
	level = 0,	-- Light intensity (9 = utevo vis lux)
	color = 0	-- Light colour (0-255)
}

monster.voices = {
	interval = 5000,                       		-- How often to try and speak
	chance = 10,                           		-- % chance to speak per interval
	{ text = "I'm an example!", yell = false },
	{ text = "You shall bow!",  yell = true }	-- yell = text can be seen over multiple floors and from further away
}

monster.summons = {
	{
		name = "demon skeleton",             		-- Monster name to summon
		chance = 20,                         		-- % chance per interval to spawn the summon
		interval = 2000,                     		-- 1000 = 1 second
		max = 4,                             		-- Max of this type (still limited by maxSummons)
		effect = CONST_ME_EXPLOSIONHIT,      		-- Effect on summon when summoned CONST_ME_NONE is also valid
		masterEffect = CONST_ME_CONST_ME_MAGIC_RED	-- Effect on master when summoning
	}
}

monster.loot = {
	{ id = "gold coin", chance = 100000, maxCount = 100 }, -- 100000 = 100% chance
	{
		id = "magic sword",
		chance = 100000,
		maxCount = 1, -- Counts only apply for stackable items
		aid = 1234, -- Set action id for this item
		-- charges = 10,          			-- Set charges, overriding the default charges of the item (if charges defined in items.toml)
	},
	{
		id = 1987, -- Can use id or name for loot, in this case there is more than 1 item called "bag" so we must use id
		chance = 100000,
		child = {
			-- inside the bag
			{ id = "platinum coin", chance = 100000, maxCount = 10 },
			{ id = "crystal coin",  chance = 100000, maxCount = 5 }
		}
	}
}

monster.attacks = {
	{
		name = "melee",
		attack = 80,          		-- Formula: skill * (attack * 0.05) + (attack * 0.5)
		skill = 70,           		-- Can use min/maxDamage to have more predictable and consistent damage
		interval = 2000,      		-- Attack interval (1000 = 1 second)
		effect = CONST_ME_DRAWBLOOD -- Combat effect (on target)
	},
	{
		name = "combat",
		type = COMBAT_FIREDAMAGE,
		chance = 20,           			-- Chance per interval to attack
		interval = 2000,
		minDamage = -150,      			-- Damage must be negative
		maxDamage = -250,
		range = 7,             			-- Max range
		radius = 1,            			-- AoE radius around target
		target = true,         			-- Requires target
		effect = CONST_ME_FIREAREA, 	-- Effect on target
		shootEffect = CONST_ANI_FIRE 	-- Effect of the projectile
	},
	{
		name = "combat",
		type = COMBAT_ENERGYDAMAGE,
		chance = 15,
		interval = 2000,
		minDamage = -200,
		maxDamage = -350,
		length = 8, -- Wave length
		spread = 0, -- Wave spread (dragon fire wave = 8 length, 3 spread)
		effect = CONST_ME_ENERGYAREA
	},
	{
		name = "combat",
		type = COMBAT_FIREDAMAGE,
		chance = 10,
		interval = 2000,
		minDamage = -100, -- Initial damage
		maxDamage = -150,
		range = 7,
		target = true,
		condition = {
			type = CONDITION_FIRE,
			minDamage = 5, -- Damage per interval
			maxDamage = 10,
			duration = 10000,
			interval = 1000
		}
	},
	{
		name = "speed",
		chance = 15,
		interval = 2000,
		speed = -700, -- Negative value = paralyze(slow down), positive = haste(speed up)
		radius = 3,
		duration = 15000,
		effect = CONST_ME_MAGIC_RED
	},
	{
		name = "firefield", -- Shoot fire field at target
		chance = 10,
		interval = 2000,
		range = 7,
		radius = 2,
		target = true,
		shootEffect = CONST_ANI_FIRE
	},
	{
		script = "monster_spell_name.lua", -- Spell script
		chance = 15,
		interval = 3000,
		minDamage = -200,
		maxDamage = -400,
		target = true
	}
}

monster.defenses = {
	defense = 45,
	armor = 40,
	{
		name = "combat",
		type = COMBAT_HEALING,
		chance = 15,
		interval = 2000,
		minDamage = 100, -- Positive damage = healing
		maxDamage = 200,
		effect = CONST_ME_MAGIC_BLUE
	},
	{
		name = "speed",
		chance = 15,
		interval = 2000,
		speed = 400,
		duration = 5000,
		effect = CONST_ME_MAGIC_GREEN
	},
	{
		name = "invisible",
		chance = 10,
		interval = 2000,
		duration = 3000,
		effect = CONST_ME_MAGIC_BLUE
	}
}

monster.elements = {
	-- 100% resistance is NOT immunity. Monsters and spell/damage effects behave differently when immune vs 100% resistant
	{ type = COMBAT_PHYSICALDAMAGE, percent = 10 },
	{ type = COMBAT_ENERGYDAMAGE,   percent = 50 },
	{ type = COMBAT_EARTHDAMAGE,    percent = 0 },
	{ type = COMBAT_FIREDAMAGE,     percent = -25 }, -- 25% fire weakness
	{ type = COMBAT_ICEDAMAGE,      percent = 100 }, -- 100% ice resistance
	{ type = COMBAT_HOLYDAMAGE,     percent = 0 },
	{ type = COMBAT_DEATHDAMAGE,    percent = 0 },
	{ type = COMBAT_DROWNDAMAGE,    percent = 0 },
	{ type = COMBAT_LIFEDRAIN,      percent = 0 },
	{ type = COMBAT_MANADRAIN,      percent = 0 }
}

monster.immunities = {

	-- immunities for direct damage
	{ type = "physical",  combat = false },
	{ type = "energy",    combat = true },
	{ type = "fire",      combat = false },
	{ type = "poison",    combat = true },   	-- "earth"
	{ type = "drown",     combat = false },
	{ type = "ice",       combat = true },
	{ type = "holy",      combat = false },
	{ type = "death",     combat = true },
	{ type = "lifedrain", combat = false },
	{ type = "manadrain", combat = true },

	-- immunities for conditions
	{ type = "physical",  condition = false },	-- "bleed"
	{ type = "energy",    condition = true },
	{ type = "fire",      condition = false },
	{ type = "poison",    condition = true },   -- "earth"
	{ type = "drown",     condition = false },
	{ type = "ice",       condition = true },
	{ type = "holy",      condition = false },
	{ type = "death",     condition = true },
	{ type = "paralyze",  condition = false },
	{ type = "outfit",    condition = true },
	{ type = "drunk",     condition = false },
	{ type = "invisible", condition = true }   -- "invisibility"

	-- immunities for both direct damage and conditions on the same line
	-- { type = "poison",      combat = true,  condition = true }

}

monster.events = {
	--[[
	"MonsterEventName1",
	"MonsterEventName2",
	"MonsterEventName3"
	--]]
}


mType.onThink = function(monster, interval)
	-- Called every onThink interval
end

mType.onAppear = function(monster, creature)
	-- Called when creature appears to monster
end

mType.onDisappear = function(monster, creature)
	-- Called when creature disappears from monster
end

mType.onMove = function(monster, creature, fromPosition, toPosition)
	-- Called when creature moves
end

mType.onSay = function(monster, creature, type, message)
	-- Called when creature speaks nearby
end

mType:register(monster)
