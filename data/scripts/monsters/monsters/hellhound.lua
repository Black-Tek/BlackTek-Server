local mtype = Game.createMonsterType("Hellhound")
local monster = {}

monster.name = "Hellhound"
monster.description = "a hellhound"
monster.experience = 5440
monster.race = "blood"
monster.maxHealth = 7500
monster.health = 7500
monster.speed = 360
monster.manaCost = 0
monster.corpse = 6332
monster.outfit = { lookType = 240 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -520,
        -- NOTE: melee condition (poison=800); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 800000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 5,
        minDamage = -300,
        maxDamage = -700,
        length = 8,
        spread = 3,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -395,
        maxDamage = -498,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -350,
        maxDamage = -660,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -350,
        maxDamage = -976,
        length = 8,
        spread = 3,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -403,
        radius = 1,
        target = true,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -300,
        maxDamage = -549,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 60,
    armor = 60,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 320,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 220,
        maxDamage = 425,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GROOOWL!", yell = false},
    {text = "GRRRRR!", yell = false},
}
monster.loot = {
    {
        id = 2152,
        chance = 100000,
        maxCount = 7,
    },
    {
        id = 2671,
        chance = 30654,
    },
    {
        id = 7590,
        chance = 29039,
        maxCount = 3,
    },
    {
        id = 7368,
        chance = 24901,
        maxCount = 10,
    },
    {
        id = 6500,
        chance = 21552,
    },
    {
        id = 10554,
        chance = 20449,
    },
    {
        id = 6558,
        chance = 20331,
    },
    {
        id = 8472,
        chance = 19819,
    },
    {
        id = 5944,
        chance = 19622,
    },
    {
        id = 8473,
        chance = 15288,
    },
    {
        id = 18425,
        chance = 13396,
    },
    {
        id = 2149,
        chance = 11072,
        maxCount = 3,
    },
    {
        id = 5925,
        chance = 10914,
    },
    {
        id = 9970,
        chance = 10402,
        maxCount = 3,
    },
    {
        id = 10553,
        chance = 9890,
    },
    {
        id = 2147,
        chance = 9614,
        maxCount = 3,
    },
    {
        id = 2144,
        chance = 9456,
        maxCount = 4,
    },
    {
        id = 2187,
        chance = 8353,
    },
    {
        id = 2430,
        chance = 7486,
    },
    {
        id = 2392,
        chance = 6422,
    },
    {
        id = 5914,
        chance = 5792,
    },
    {
        id = 5910,
        chance = 5280,
    },
    {
        id = 2156,
        chance = 4610,
    },
    {
        id = 2154,
        chance = 4452,
    },
    {
        id = 5911,
        chance = 3034,
    },
    {
        id = 9971,
        chance = 2955,
    },
    {
        id = 7890,
        chance = 2561,
    },
    {
        id = 7426,
        chance = 2049,
    },
    {
        id = 7894,
        chance = 1458,
    },
    {
        id = 7891,
        chance = 1340,
    },
    {
        id = 2231,
        chance = 985,
    },
    {
        id = 2155,
        chance = 985,
    },
    {
        id = 7899,
        chance = 985,
    },
    {
        id = 7900,
        chance = 985,
    },
    {
        id = 7421,
        chance = 867,
    },
    {
        id = 2393,
        chance = 827,
    },
    {
        id = 6553,
        chance = 788,
    },
    {
        id = 4873,
        chance = 276,
    },
}

mtype:register(monster)
