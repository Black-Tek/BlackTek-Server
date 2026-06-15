local mtype = Game.createMonsterType("Frazzlemaw")
local monster = {}

monster.name = "Frazzlemaw"
monster.description = "a frazzlemaw"
monster.experience = 3400
monster.race = "blood"
monster.maxHealth = 4100
monster.health = 4100
monster.speed = 400
monster.manaCost = 0
monster.corpse = 22567
monster.outfit = { lookType = 594 }
monster.changeTarget = {
    interval = 4000,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -400,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 10,
        minDamage = -300,
        maxDamage = -400,
        radius = 3,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -700,
        length = 5,
        target = true,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = 0,
        maxDamage = -400,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_STONES,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 5,
        target = false,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -80,
        maxDamage = -150,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 74,
    armor = 74,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 250,
        maxDamage = 425,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Mwaaahgod! Overmwaaaaah! *gurgle*", yell = false},
    {text = "Mwaaaahnducate youuuuuu *gurgle*, mwaaah!", yell = false},
    {text = "MMMWAHMWAHMWAHMWAAAAH!", yell = false},
    {text = "Mmmwhamwhamwhah, mwaaah!", yell = false},
}
monster.loot = {
    {
        id = 2152,
        chance = 90539,
        maxCount = 7,
    },
    {
        id = 2148,
        chance = 78242,
        maxCount = 100,
    },
    {
        id = 22532,
        chance = 14245,
    },
    {
        id = 22533,
        chance = 12119,
    },
    {
        id = 7590,
        chance = 12048,
        maxCount = 3,
    },
    {
        id = 18417,
        chance = 11977,
    },
    {
        id = 7591,
        chance = 11836,
        maxCount = 2,
    },
    {
        id = 2226,
        chance = 10064,
    },
    {
        id = 2229,
        chance = 9497,
    },
    {
        id = 5951,
        chance = 8717,
    },
    {
        id = 2230,
        chance = 8079,
    },
    {
        id = 2225,
        chance = 7760,
    },
    {
        id = 2240,
        chance = 7548,
    },
    {
        id = 18554,
        chance = 7512,
    },
    {
        id = 2219,
        chance = 7335,
    },
    {
        id = 2667,
        chance = 5067,
        maxCount = 3,
    },
    {
        id = 5895,
        chance = 4465,
    },
    {
        id = 2671,
        chance = 4394,
    },
    {
        id = 18420,
        chance = 4146,
    },
    {
        id = 5925,
        chance = 3792,
    },
    {
        id = 2231,
        chance = 3650,
    },
    {
        id = 5880,
        chance = 2729,
    },
    {
        id = 18414,
        chance = 2516,
    },
    {
        id = 2377,
        chance = 2303,
    },
    {
        id = 9971,
        chance = 1914,
    },
    {
        id = 7407,
        chance = 1453,
    },
    {
        id = 11306,
        chance = 886,
    },
    {
        id = 22396,
        chance = 532,
    },
    {
        id = 7418,
        chance = 532,
    },
    {
        id = 7404,
        chance = 390,
    },
}

mtype:register(monster)
