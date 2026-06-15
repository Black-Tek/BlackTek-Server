local mtype = Game.createMonsterType("Thornback Tortoise")
local monster = {}

monster.name = "Thornback Tortoise"
monster.description = "a thornback tortoise"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 300
monster.health = 300
monster.speed = 200
monster.manaCost = 490
monster.corpse = 6073
monster.outfit = { lookType = 198 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
        -- NOTE: melee condition (poison=40); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 40000,
        },
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 45},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.loot = {
    {
        id = "white pearl",
        chance = 1600,
    },
    {
        id = "black pearl",
        chance = 800,
    },
    {
        id = "gold coin",
        chance = 89500,
        maxCount = 48,
    },
    {
        id = "war hammer",
        chance = 260,
    },
    {
        id = 2667,
        chance = 10800,
        maxCount = 2,
    },
    {
        id = "white mushroom",
        chance = 1200,
    },
    {
        id = "brown mushroom",
        chance = 700,
    },
    {
        id = "tortoise egg",
        chance = 790,
        maxCount = 3,
    },
    {
        id = "turtle shell",
        chance = 800,
    },
    {
        id = "health potion",
        chance = 1600,
    },
    {
        id = "thorn",
        chance = 15980,
    },
}

mtype:register(monster)
