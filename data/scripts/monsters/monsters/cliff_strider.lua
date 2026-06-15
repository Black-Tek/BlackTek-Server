local mtype = Game.createMonsterType("Cliff Strider")
local monster = {}

monster.name = "Cliff Strider"
monster.description = "a cliff strider"
monster.experience = 5700
monster.race = "undead"
monster.maxHealth = 9400
monster.health = 9400
monster.speed = 246
monster.manaCost = 0
monster.corpse = 17420
monster.outfit = { lookType = 497 }
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -499,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -800,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_STONES,
    },
    {
        name = "cliff strider skill reducer",
        interval = 2000,
        chance = 10,
    },
    {
        name = "cliff strider electrify",
        interval = 2000,
        chance = 15,
        range = 1,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -1000,
        length = 6,
        target = false,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        minDamage = -100,
        maxDamage = -300,
        radius = 4,
        target = false,
        effect = CONST_ME_YELLOWENERGY,
    },
}
monster.defenses = {
    defense = 89,
    armor = 89,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 35},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Knorrrr", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 196,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 10,
    },
    {
        id = 7590,
        chance = 33641,
        maxCount = 4,
    },
    {
        id = 8473,
        chance = 24406,
        maxCount = 2,
    },
    {
        id = 18429,
        chance = 17942,
        maxCount = 2,
    },
    {
        id = 5944,
        chance = 16887,
    },
    {
        id = 18427,
        chance = 15765,
    },
    {
        id = 18428,
        chance = 15369,
    },
    {
        id = 5880,
        chance = 13918,
    },
    {
        id = 11227,
        chance = 12071,
    },
    {
        id = 2144,
        chance = 9301,
    },
    {
        id = 18419,
        chance = 9301,
    },
    {
        id = 18435,
        chance = 9169,
        maxCount = 8,
    },
    {
        id = 2143,
        chance = 8971,
        maxCount = 3,
    },
    {
        id = 18418,
        chance = 8905,
        maxCount = 2,
    },
    {
        id = 18413,
        chance = 6728,
    },
    {
        id = 2156,
        chance = 5739,
    },
    {
        id = 9942,
        chance = 2704,
    },
    {
        id = 18390,
        chance = 1913,
    },
    {
        id = 7437,
        chance = 1517,
    },
    {
        id = 5904,
        chance = 1451,
    },
    {
        id = 18412,
        chance = 1187,
    },
    {
        id = 7452,
        chance = 1055,
    },
    {
        id = 18453,
        chance = 923,
    },
    {
        id = 2477,
        chance = 858,
    },
    {
        id = 9980,
        chance = 792,
    },
    {
        id = 2158,
        chance = 726,
    },
    {
        id = 2393,
        chance = 660,
    },
    {
        id = 18450,
        chance = 594,
    },
    {
        id = 2497,
        chance = 528,
    },
    {
        id = 2487,
        chance = 462,
    },
    {
        id = 2645,
        chance = 66,
    },
}

mtype:register(monster)
