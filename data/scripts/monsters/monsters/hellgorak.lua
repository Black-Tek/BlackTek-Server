local mtype = Game.createMonsterType("Hellgorak")
local monster = {}

monster.name = "Hellgorak"
monster.description = "Hellgorak"
monster.experience = 10000
monster.race = "fire"
monster.maxHealth = 25850
monster.health = 25850
monster.speed = 330
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 0,
    lookBody = 77,
    lookLegs = 1,
    lookFeet = 3,
}
monster.runHealth = 150
monster.changeTarget = {
    interval = 5000,
    chance = 28,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
        maxDamage = -910,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 11,
        minDamage = -250,
        maxDamage = -850,
        length = 8,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 14,
        minDamage = -200,
        maxDamage = -400,
        radius = 5,
        target = false,
        effect = CONST_ME_STUN,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 11,
        range = 7,
        minDamage = -50,
        maxDamage = -600,
        radius = 5,
        target = true,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 5,
        minDamage = 0,
        maxDamage = -150,
        radius = 7,
        target = false,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 65,
    armor = 70,
    {
        name = "healing",
        interval = 2000,
        chance = 22,
        minDamage = 400,
        maxDamage = 1700,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 98},
    {type = COMBAT_ENERGYDAMAGE, percent = 98},
    {type = COMBAT_EARTHDAMAGE, percent = 98},
    {type = COMBAT_FIREDAMAGE, percent = 98},
    {type = COMBAT_DROWNDAMAGE, percent = -205},
    {type = COMBAT_ICEDAMAGE, percent = 98},
    {type = COMBAT_HOLYDAMAGE, percent = 95},
    {type = COMBAT_DEATHDAMAGE, percent = 98},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I'll sacrifice yours souls to seven!", yell = false},
    {text = "I'm bad news for you mortals!", yell = false},
    {text = "No man can defeat me!", yell = false},
    {text = "Your puny skills are no match for me.", yell = false},
    {text = "I smell your fear.", yell = false},
    {text = "Delicious!", yell = false},
}
monster.loot = {
    {
        id = 6500,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 198,
    },
    {
        id = 9813,
        chance = 51393,
    },
    {
        id = 8473,
        chance = 42605,
        maxCount = 2,
    },
    {
        id = 9810,
        chance = 32476,
    },
    {
        id = 8901,
        chance = 30493,
    },
    {
        id = 3962,
        chance = 29957,
    },
    {
        id = 8472,
        chance = 21115,
    },
    {
        id = 2152,
        chance = 20579,
        maxCount = 30,
    },
    {
        id = 2487,
        chance = 18435,
    },
    {
        id = 7591,
        chance = 18274,
    },
    {
        id = 7590,
        chance = 17899,
    },
    {
        id = 2125,
        chance = 14362,
    },
    {
        id = 2144,
        chance = 14148,
        maxCount = 25,
    },
    {
        id = 2147,
        chance = 13130,
        maxCount = 25,
    },
    {
        id = 2143,
        chance = 13076,
        maxCount = 25,
    },
    {
        id = 2145,
        chance = 12808,
        maxCount = 25,
    },
    {
        id = 7456,
        chance = 12165,
    },
    {
        id = 2150,
        chance = 12004,
        maxCount = 25,
    },
    {
        id = 2149,
        chance = 11790,
        maxCount = 25,
    },
    {
        id = 2146,
        chance = 11576,
        maxCount = 25,
    },
    {
        id = 9970,
        chance = 11415,
        maxCount = 25,
    },
    {
        id = 7894,
        chance = 11093,
    },
    {
        id = 8870,
        chance = 10343,
    },
    {
        id = 2133,
        chance = 10129,
    },
    {
        id = 8902,
        chance = 10021,
    },
    {
        id = 2488,
        chance = 9914,
    },
    {
        id = 2197,
        chance = 9753,
    },
    {
        id = 2130,
        chance = 9700,
    },
    {
        id = 5954,
        chance = 9539,
        maxCount = 2,
    },
    {
        id = 2645,
        chance = 9539,
    },
    {
        id = 8871,
        chance = 9486,
    },
    {
        id = 8903,
        chance = 8896,
    },
    {
        id = 2477,
        chance = 8789,
    },
    {
        id = 2656,
        chance = 8039,
    },
    {
        id = 7412,
        chance = 2304,
    },
    {
        id = 2466,
        chance = 2036,
    },
    {
        id = 7388,
        chance = 1179,
    },
    {
        id = 8904,
        chance = 1018,
    },
    {
        id = 7453,
        chance = 804,
    },
    {
        id = 2470,
        chance = 643,
    },
    {
        id = 2136,
        chance = 589,
    },
    {
        id = 8879,
        chance = 375,
    },
    {
        id = 8926,
        chance = 322,
    },
    {
        id = 2415,
        chance = 268,
    },
    {
        id = 8918,
        chance = 161,
    },
}

mtype:register(monster)
