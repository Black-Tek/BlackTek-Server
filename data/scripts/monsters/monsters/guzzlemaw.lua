local mtype = Game.createMonsterType("Guzzlemaw")
local monster = {}

monster.name = "Guzzlemaw"
monster.description = "a guzzlemaw"
monster.experience = 6050
monster.race = "blood"
monster.maxHealth = 6400
monster.health = 6400
monster.speed = 400
monster.manaCost = 0
monster.corpse = 22485
monster.outfit = { lookType = 584 }
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
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -550,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 10,
        minDamage = -500,
        maxDamage = -1000,
        radius = 3,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -900,
        length = 8,
        spread = 3,
        target = true,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -500,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_STONES,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 6,
        target = false,
        speed = -800,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -120,
        maxDamage = -250,
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
        chance = 20,
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
    interval = 2000,
    chance = 5,
    {text = "Gmmmooooh! *chomp*", yell = false},
    {text = "MWAAAH! *gurgle*", yell = false},
    {text = "*chomp* Mmmoh! *chomp*", yell = false},
}
monster.loot = {
    {
        id = 2152,
        chance = 87952,
        maxCount = 7,
    },
    {
        id = 2148,
        chance = 75663,
        maxCount = 100,
    },
    {
        id = 7590,
        chance = 15261,
        maxCount = 3,
    },
    {
        id = 7591,
        chance = 11968,
        maxCount = 2,
    },
    {
        id = 22532,
        chance = 10281,
    },
    {
        id = 2229,
        chance = 9719,
    },
    {
        id = 22533,
        chance = 9157,
    },
    {
        id = 2226,
        chance = 8353,
    },
    {
        id = 5951,
        chance = 7952,
    },
    {
        id = 2225,
        chance = 7871,
    },
    {
        id = 2230,
        chance = 7791,
    },
    {
        id = 2240,
        chance = 7791,
    },
    {
        id = 2671,
        chance = 7550,
    },
    {
        id = 18554,
        chance = 7229,
    },
    {
        id = 2219,
        chance = 7149,
    },
    {
        id = 18417,
        chance = 6827,
        maxCount = 2,
    },
    {
        id = 18420,
        chance = 5542,
    },
    {
        id = 2667,
        chance = 4498,
        maxCount = 3,
    },
    {
        id = 2231,
        chance = 4016,
    },
    {
        id = 5895,
        chance = 3614,
    },
    {
        id = 5925,
        chance = 3052,
    },
    {
        id = 2377,
        chance = 2249,
    },
    {
        id = 5880,
        chance = 2169,
    },
    {
        id = 18414,
        chance = 2088,
    },
    {
        id = 22396,
        chance = 1526,
    },
    {
        id = 7407,
        chance = 1446,
    },
    {
        id = 11306,
        chance = 1285,
    },
    {
        id = 7404,
        chance = 643,
    },
    {
        id = 7418,
        chance = 643,
    },
}

mtype:register(monster)
