local mtype = Game.createMonsterType("Swamp Troll")
local monster = {}

monster.name = "Swamp Troll"
monster.description = "a swamp troll"
monster.experience = 25
monster.race = "blood"
monster.maxHealth = 55
monster.health = 55
monster.speed = 128
monster.manaCost = 320
monster.corpse = 6018
monster.outfit = { lookType = 76 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -13,
        -- NOTE: melee condition (poison=1); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 1000,
        },
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrr", yell = false},
    {text = "Groar!", yell = false},
    {text = "Me strong! Me ate spinach!", yell = false},
}
monster.loot = {
    {
        id = 2667,
        chance = 60000,
    },
    {
        id = "gold coin",
        chance = 50300,
        maxCount = 5,
    },
    {
        id = 2050,
        chance = 15000,
    },
    {
        id = "spear",
        chance = 13000,
    },
    {
        id = "mouldy cheese",
        chance = 10000,
    },
    {
        id = "leather boots",
        chance = 10000,
    },
    {
        id = "swamp grass",
        chance = 3100,
    },
    {
        id = "medicine pouch",
        chance = 2400,
    },
    {
        id = 5901,
        chance = 2000,
    },
    {
        id = "troll green",
        chance = 1000,
    },
    {
        id = "fishing rod",
        chance = 80,
    },
}

mtype:register(monster)
