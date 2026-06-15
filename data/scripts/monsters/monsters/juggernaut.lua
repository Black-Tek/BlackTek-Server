local mtype = Game.createMonsterType("Juggernaut")
local monster = {}

monster.name = "Juggernaut"
monster.description = "a juggernaut"
monster.experience = 11200
monster.race = "blood"
monster.maxHealth = 20000
monster.health = 20000
monster.speed = 340
monster.manaCost = 0
monster.corpse = 6336
monster.outfit = { lookType = 244 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 60
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1470,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -780,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 60,
    armor = 60,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 520,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 400,
        maxDamage = 900,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "RAAARRR!", yell = false},
    {text = "GRRRRRR!", yell = false},
    {text = "WAHHHH!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 87649,
        maxCount = 198,
    },
    {
        id = 2152,
        chance = 85549,
        maxCount = 15,
    },
    {
        id = 2671,
        chance = 44730,
    },
    {
        id = 6500,
        chance = 30098,
    },
    {
        id = 5944,
        chance = 23144,
    },
    {
        id = 7591,
        chance = 21152,
        maxCount = 3,
    },
    {
        id = 7590,
        chance = 20645,
        maxCount = 3,
    },
    {
        id = 6558,
        chance = 19775,
    },
    {
        id = 7368,
        chance = 16661,
        maxCount = 10,
    },
    {
        id = 2149,
        chance = 13654,
        maxCount = 5,
    },
    {
        id = 9970,
        chance = 13003,
        maxCount = 5,
    },
    {
        id = 2147,
        chance = 12821,
        maxCount = 5,
    },
    {
        id = 2150,
        chance = 12677,
        maxCount = 5,
    },
    {
        id = 2145,
        chance = 12387,
        maxCount = 5,
    },
    {
        id = 7365,
        chance = 10286,
        maxCount = 15,
    },
    {
        id = 2156,
        chance = 9381,
    },
    {
        id = 7632,
        chance = 6918,
    },
    {
        id = 9971,
        chance = 5179,
        maxCount = 2,
    },
    {
        id = 7413,
        chance = 4745,
    },
    {
        id = 2476,
        chance = 4382,
    },
    {
        id = 7452,
        chance = 4020,
    },
    {
        id = 2155,
        chance = 2390,
    },
    {
        id = 2454,
        chance = 1594,
    },
    {
        id = 2153,
        chance = 942,
    },
    {
        id = 2514,
        chance = 435,
    },
    {
        id = 2136,
        chance = 362,
    },
    {
        id = 2466,
        chance = 326,
    },
    {
        id = 2452,
        chance = 326,
    },
    {
        id = 2578,
        chance = 181,
    },
}

mtype:register(monster)
