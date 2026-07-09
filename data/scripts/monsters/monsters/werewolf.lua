local mtype = Game.createMonsterType("Werewolf")
local monster = {}

monster.name = "Werewolf"
monster.description = "a werewolf"
monster.experience = 1900
monster.race = "blood"
monster.maxHealth = 1955
monster.health = 1955
monster.speed = 280
monster.manaCost = 0
monster.corpse = 20380
monster.outfit = { lookType = 308 }
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -350,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        radius = 1,
        target = true,
        duration = 2000,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -80,
        maxDamage = -200,
        length = 4,
        spread = 2,
        target = false,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        radius = 3,
        target = false,
        effect = CONST_ME_SOUND_WHITE,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        radius = 1,
        target = false,
        effect = CONST_ME_SOUND_GREEN,
    },
    {
        name = "werewolf skill reducer",
        interval = 2000,
        chance = 15,
        range = 1,
    },
}
monster.defenses = {
    defense = 36,
    armor = 36,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 120,
        maxDamage = 225,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = 400,
        duration = 5000,
        effect = CONST_ME_SOUND_PURPLE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_EARTHDAMAGE, percent = 75},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 55},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "war wolf", interval = 2000, chance = 40, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "BLOOD!", yell = true},
    {text = "HRAAAAAAAAAARRRRRR!", yell = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 98760,
        maxCount = 301,
    },
    {
        id = 24708,
        chance = 14788,
    },
    {
        id = 11234,
        chance = 9757,
    },
    {
        id = 2510,
        chance = 9645,
        maxCount = 2,
    },
    {
        id = 9809,
        chance = 7958,
    },
    {
        id = 2789,
        chance = 6903,
    },
    {
        id = 5897,
        chance = 5211,
    },
    {
        id = 7588,
        chance = 5171,
        maxCount = 2,
    },
    {
        id = 2805,
        chance = 4987,
    },
    {
        id = 2381,
        chance = 3072,
    },
    {
        id = 8473,
        chance = 2016,
    },
    {
        id = 7439,
        chance = 938,
    },
    {
        id = 2197,
        chance = 882,
    },
    {
        id = 2171,
        chance = 799,
    },
    {
        id = 24739,
        chance = 726,
    },
    {
        id = 2169,
        chance = 581,
    },
    {
        id = 2438,
        chance = 475,
    },
    {
        id = 7383,
        chance = 363,
    },
    {
        id = 7428,
        chance = 268,
    },
    {
        id = 7419,
        chance = 106,
    },
}

mtype:register(monster)
