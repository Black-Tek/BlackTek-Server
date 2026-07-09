local mtype = Game.createMonsterType("Dragon")
local monster = {}

monster.name = "Dragon"
monster.description = "a dragon"
monster.experience = 700
monster.race = "blood"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 172
monster.manaCost = 0
monster.corpse = 5973
monster.outfit = { lookType = 34 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -140,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -170,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 40,
        maxDamage = 70,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GROOAAARRR", yell = true},
    {text = "FCHHHHH", yell = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 90082,
        maxCount = 105,
    },
    {
        id = 2672,
        chance = 65143,
    },
    {
        id = 2509,
        chance = 14893,
    },
    {
        id = 2455,
        chance = 10085,
    },
    {
        id = 12413,
        chance = 9883,
    },
    {
        id = 2546,
        chance = 7976,
        maxCount = 10,
    },
    {
        id = 2397,
        chance = 4027,
    },
    {
        id = 3351,
        chance = 3005,
    },
    {
        id = 2413,
        chance = 1995,
    },
    {
        id = 2647,
        chance = 1909,
    },
    {
        id = 7588,
        chance = 1055,
    },
    {
        id = 2187,
        chance = 1053,
    },
    {
        id = 5920,
        chance = 1038,
    },
    {
        id = 5877,
        chance = 1018,
    },
    {
        id = 2387,
        chance = 1008,
    },
    {
        id = 2434,
        chance = 517,
    },
    {
        id = 2409,
        chance = 504,
    },
    {
        id = 2145,
        chance = 384,
    },
    {
        id = 2516,
        chance = 301,
    },
    {
        id = 2177,
        chance = 113,
    },
    {
        id = 7430,
        chance = 102,
    },
}

mtype:register(monster)
