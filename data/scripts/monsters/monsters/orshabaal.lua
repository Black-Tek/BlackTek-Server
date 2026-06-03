local mtype = Game.createMonsterType("Orshabaal")
local monster = {}

monster.name = "Orshabaal"
monster.description = "Orshabaal"
monster.experience = 10000
monster.race = "fire"
monster.maxHealth = 22500
monster.health = 22500
monster.speed = 380
monster.manaCost = 0
monster.corpse = 2916
monster.outfit = { lookType = 201 }
monster.runHealth = 2500
monster.changeTarget = {
    interval = 2000,
    chance = 10,
}
monster.targetDistance = 1
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1990,
    },
    {
        name = "manadrain",
        interval = 1000,
        chance = 13,
        range = 7,
        minDamage = -300,
        maxDamage = -600,
    },
    {
        name = "manadrain",
        interval = 1000,
        chance = 6,
        minDamage = -150,
        maxDamage = -350,
        radius = 5,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "effect",
        interval = 1000,
        chance = 6,
        radius = 5,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 34,
        range = 7,
        minDamage = -310,
        maxDamage = -600,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 1000,
        chance = 10,
        range = 7,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 15,
        minDamage = -500,
        maxDamage = -850,
        length = 8,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 90,
    armor = 90,
    {
        name = "healing",
        interval = 1000,
        chance = 9,
        minDamage = 1500,
        maxDamage = 2500,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 1000,
        chance = 17,
        minDamage = 600,
        maxDamage = 1000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 5,
        speed = 1140,
        duration = 7000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 80},
    {type = COMBAT_ICEDAMAGE, percent = -1},
    {type = COMBAT_HOLYDAMAGE, percent = -1},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "demon", interval = 1000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "PRAISED BE MY MASTERS, THE RUTHLESS SEVEN!", yell = false},
    {text = "YOU ARE DOOMED!", yell = false},
    {text = "ORSHABAAL IS BACK!", yell = false},
    {text = "Be prepared for the day my masters will come for you!", yell = false},
    {text = "SOULS FOR ORSHABAAL!", yell = false},
}
monster.loot = {
    {
        id = 2152,
        chance = 100000,
        maxCount = 72,
    },
    {
        id = 6500,
        chance = 94444,
    },
    {
        id = 6300,
        chance = 55556,
    },
    {
        id = 5954,
        chance = 50000,
        maxCount = 2,
    },
    {
        id = 7590,
        chance = 33333,
    },
    {
        id = 2146,
        chance = 33333,
        maxCount = 8,
    },
    {
        id = 2143,
        chance = 33333,
        maxCount = 15,
    },
    {
        id = 2144,
        chance = 27778,
        maxCount = 15,
    },
    {
        id = 2462,
        chance = 27778,
    },
    {
        id = 7591,
        chance = 27778,
        maxCount = 5,
    },
    {
        id = 2214,
        chance = 27778,
    },
    {
        id = 8473,
        chance = 27778,
    },
    {
        id = 2158,
        chance = 22222,
    },
    {
        id = 2520,
        chance = 22222,
    },
    {
        id = 2393,
        chance = 22222,
    },
    {
        id = 2178,
        chance = 22222,
    },
    {
        id = 2145,
        chance = 22222,
        maxCount = 5,
    },
    {
        id = 2149,
        chance = 22222,
        maxCount = 7,
    },
    {
        id = 7368,
        chance = 16667,
        maxCount = 42,
    },
    {
        id = 2195,
        chance = 16667,
    },
    {
        id = 2125,
        chance = 16667,
    },
    {
        id = 2200,
        chance = 16667,
    },
    {
        id = 1982,
        chance = 16667,
    },
    {
        id = 2170,
        chance = 16667,
    },
    {
        id = 2150,
        chance = 16667,
        maxCount = 17,
    },
    {
        id = 2174,
        chance = 16667,
    },
    {
        id = 2151,
        chance = 16667,
        maxCount = 3,
    },
    {
        id = 2377,
        chance = 16667,
    },
    {
        id = 2432,
        chance = 11111,
    },
    {
        id = 2470,
        chance = 11111,
    },
    {
        id = 2033,
        chance = 11111,
    },
    {
        id = 8472,
        chance = 11111,
    },
    {
        id = 2177,
        chance = 11111,
    },
    {
        id = 2514,
        chance = 11111,
    },
    {
        id = 2176,
        chance = 11111,
    },
    {
        id = 2171,
        chance = 11111,
    },
    {
        id = 2197,
        chance = 11111,
    },
    {
        id = 2142,
        chance = 5556,
    },
    {
        id = 2434,
        chance = 5556,
    },
    {
        id = 9971,
        chance = 5556,
    },
    {
        id = 2418,
        chance = 5556,
    },
    {
        id = 2155,
        chance = 5556,
    },
    {
        id = 2162,
        chance = 5556,
    },
    {
        id = 2472,
        chance = 5556,
    },
    {
        id = 2164,
        chance = 5556,
    },
    {
        id = 7365,
        chance = 5556,
        maxCount = 50,
    },
    {
        id = 5808,
        chance = 5556,
    },
    {
        id = 2402,
        chance = 5556,
    },
    {
        id = 2112,
        chance = 5556,
    },
    {
        id = 2421,
        chance = 5556,
    },
    {
        id = 3955,
        chance = 5556,
    },
}

mtype:register(monster)
