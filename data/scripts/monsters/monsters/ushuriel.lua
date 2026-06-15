local mtype = Game.createMonsterType("Ushuriel")
local monster = {}

monster.name = "Ushuriel"
monster.description = "Ushuriel"
monster.experience = 10000
monster.race = "fire"
monster.maxHealth = 31500
monster.health = 31500
monster.speed = 440
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 0,
    lookBody = 57,
    lookLegs = 0,
    lookFeet = 61,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
        maxDamage = -1088,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 10,
        minDamage = -250,
        maxDamage = -500,
        length = 10,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "death",
        interval = 1000,
        chance = 8,
        minDamage = -30,
        maxDamage = -760,
        radius = 5,
        target = false,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 9,
        minDamage = -200,
        maxDamage = -585,
        length = 8,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 8,
        minDamage = 0,
        maxDamage = -430,
        radius = 6,
        target = false,
        effect = CONST_ME_ICETORNADO,
    },
    {
        name = "drunk",
        interval = 3000,
        chance = 11,
        radius = 6,
        target = false,
        effect = CONST_ME_SOUND_PURPLE,
    },
    {
        name = "energycondition",
        interval = 2000,
        chance = 15,
        minDamage = -250,
        maxDamage = -250,
        radius = 4,
        target = false,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 45,
    armor = 50,
    {
        name = "healing",
        interval = 1000,
        chance = 12,
        minDamage = 400,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 4,
        speed = 400,
        duration = 7000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You can't run or hide forever!", yell = false},
    {text = "I'm the executioner of the Seven!", yell = false},
    {text = "The final punishment awaits you!", yell = false},
    {text = "The judgement is guilty! The sentence is death!", yell = false},
}
monster.loot = {
    {
        id = 6500,
        chance = 100000,
    },
    {
        id = 2789,
        chance = 96863,
        maxCount = 30,
    },
    {
        id = 2148,
        chance = 96863,
        maxCount = 190,
    },
    {
        id = 5880,
        chance = 43922,
    },
    {
        id = 5925,
        chance = 28235,
        maxCount = 20,
    },
    {
        id = 8473,
        chance = 26667,
    },
    {
        id = 7591,
        chance = 25490,
    },
    {
        id = 2498,
        chance = 25294,
    },
    {
        id = 8472,
        chance = 23725,
    },
    {
        id = 2177,
        chance = 21176,
    },
    {
        id = 7590,
        chance = 20980,
    },
    {
        id = 5741,
        chance = 20980,
    },
    {
        id = 2152,
        chance = 20784,
        maxCount = 30,
    },
    {
        id = 9971,
        chance = 20196,
    },
    {
        id = 2178,
        chance = 20196,
    },
    {
        id = 5669,
        chance = 20000,
    },
    {
        id = 2475,
        chance = 19412,
    },
    {
        id = 7391,
        chance = 19216,
    },
    {
        id = 2392,
        chance = 18824,
    },
    {
        id = 2176,
        chance = 17647,
    },
    {
        id = 9810,
        chance = 16471,
    },
    {
        id = 5892,
        chance = 14510,
    },
    {
        id = 2393,
        chance = 11961,
    },
    {
        id = 2419,
        chance = 10980,
    },
    {
        id = 5954,
        chance = 10784,
        maxCount = 2,
    },
    {
        id = 2383,
        chance = 10196,
    },
    {
        id = 7402,
        chance = 10000,
    },
    {
        id = 7385,
        chance = 9216,
    },
    {
        id = 2491,
        chance = 9020,
    },
    {
        id = 2479,
        chance = 9020,
    },
    {
        id = 7417,
        chance = 8431,
    },
    {
        id = 5891,
        chance = 5686,
    },
    {
        id = 5884,
        chance = 5098,
    },
    {
        id = 6103,
        chance = 4118,
    },
    {
        id = 5885,
        chance = 3725,
    },
}

mtype:register(monster)
