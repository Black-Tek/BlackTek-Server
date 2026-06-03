local mtype = Game.createMonsterType("Worm Priestess")
local monster = {}

monster.name = "Worm Priestess"
monster.description = "a worm priestess"
monster.experience = 1500
monster.race = "blood"
monster.maxHealth = 1100
monster.health = 1100
monster.speed = 198
monster.manaCost = 0
monster.corpse = 23470
monster.outfit = { lookType = 613 }
monster.changeTarget = {
    interval = 2000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
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
        attack = 50,
        skill = 50,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 9,
        range = 7,
        minDamage = -50,
        maxDamage = -130,
        target = true,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 4,
        minDamage = -100,
        maxDamage = -165,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        radius = 3,
        target = true,
        speed = -900,
        effect = CONST_ME_POFF,
    },
    {
        name = "death",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -115,
        maxDamage = -200,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -200,
        maxDamage = -300,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 11,
        minDamage = 100,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 9,
        speed = 198,
        duration = 1000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 5},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "An enemy of the worm shall become his food!", yell = false},
    {text = "The great worm will swallow you!", yell = false},
    {text = "And our enemies he will swallow!", yell = false},
    {text = "The worm may guide me in this hour of darkness!", yell = false},
    {text = "From the earthy depths he comes and brings freedom!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 50,
    },
    {
        id = 2152,
        chance = 41130,
        maxCount = 3,
    },
    {
        id = 12429,
        chance = 15850,
    },
    {
        id = 7589,
        chance = 12380,
        maxCount = 3,
    },
    {
        id = 2182,
        chance = 11470,
    },
    {
        id = 7425,
        chance = 5210,
    },
    {
        id = 2050,
        chance = 4760,
    },
    {
        id = 5878,
        chance = 4640,
    },
    {
        id = 12428,
        chance = 2940,
        maxCount = 2,
    },
    {
        id = 2150,
        chance = 2500,
        maxCount = 2,
    },
    {
        id = 2149,
        chance = 2370,
        maxCount = 2,
    },
    {
        id = 2147,
        chance = 2200,
        maxCount = 2,
    },
    {
        id = 9970,
        chance = 2460,
        maxCount = 2,
    },
    {
        id = 2145,
        chance = 2430,
        maxCount = 2,
    },
    {
        id = 5911,
        chance = 1350,
    },
    {
        id = 5910,
        chance = 1400,
    },
    {
        id = 5912,
        chance = 1820,
    },
    {
        id = 8910,
        chance = 1590,
    },
    {
        id = 2154,
        chance = 560,
    },
    {
        id = 2156,
        chance = 430,
    },
    {
        id = 7401,
        chance = 160,
    },
}

mtype:register(monster)
