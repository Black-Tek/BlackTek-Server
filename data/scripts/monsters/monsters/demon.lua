local mtype = Game.createMonsterType("Demon")
local monster = {}

monster.name = "Demon"
monster.description = "a demon"
monster.experience = 6000
monster.race = "fire"
monster.maxHealth = 8200
monster.health = 8200
monster.speed = 256
monster.manaCost = 0
monster.corpse = 5995
monster.outfit = { lookType = 35 }
monster.changeTarget = {
    interval = 4000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
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
        maxDamage = -500,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -30,
        maxDamage = -120,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -150,
        maxDamage = -250,
        radius = 7,
        target = false,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -300,
        maxDamage = -480,
        length = 8,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -210,
        maxDamage = -300,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        radius = 1,
        target = true,
        speed = -700,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 44,
    armor = 44,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 80,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 320,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 25},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = -12},
    {type = COMBAT_HOLYDAMAGE, percent = -12},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "fire elemental", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Your soul will be mine!", yell = true},
    {text = "MUHAHAHAHA!", yell = true},
    {text = "CHAMEK ATH UTHUL ARAK!", yell = true},
    {text = "I SMELL FEEEEAAAAAR!", yell = true},
    {text = "Your resistance is futile!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99518,
        maxCount = 200,
    },
    {
        id = 2152,
        chance = 99513,
        maxCount = 8,
    },
    {
        id = 7590,
        chance = 25108,
        maxCount = 3,
    },
    {
        id = 8472,
        chance = 24763,
        maxCount = 3,
    },
    {
        id = 5954,
        chance = 20187,
    },
    {
        id = 8473,
        chance = 19831,
        maxCount = 3,
    },
    {
        id = 2795,
        chance = 19782,
        maxCount = 6,
    },
    {
        id = 6500,
        chance = 19738,
    },
    {
        id = 7368,
        chance = 15452,
        maxCount = 10,
    },
    {
        id = 9970,
        chance = 10167,
        maxCount = 5,
    },
    {
        id = 2147,
        chance = 10006,
        maxCount = 5,
    },
    {
        id = 2149,
        chance = 9802,
        maxCount = 5,
    },
    {
        id = 2150,
        chance = 9789,
        maxCount = 5,
    },
    {
        id = 2432,
        chance = 4037,
    },
    {
        id = 2151,
        chance = 3420,
    },
    {
        id = 2156,
        chance = 2959,
    },
    {
        id = 2176,
        chance = 2852,
    },
    {
        id = 2214,
        chance = 2658,
    },
    {
        id = 2164,
        chance = 2492,
    },
    {
        id = 2165,
        chance = 2391,
    },
    {
        id = 2393,
        chance = 1950,
    },
    {
        id = 2396,
        chance = 1906,
    },
    {
        id = 2418,
        chance = 1367,
    },
    {
        id = 1982,
        chance = 1232,
    },
    {
        id = 2462,
        chance = 1227,
    },
    {
        id = 2179,
        chance = 1035,
    },
    {
        id = 2520,
        chance = 755,
    },
    {
        id = 2171,
        chance = 705,
    },
    {
        id = 2514,
        chance = 475,
    },
    {
        id = 2470,
        chance = 397,
    },
    {
        id = 7393,
        chance = 88,
    },
    {
        id = 2472,
        chance = 88,
    },
    {
        id = 7382,
        chance = 54,
    },
}

mtype:register(monster)
