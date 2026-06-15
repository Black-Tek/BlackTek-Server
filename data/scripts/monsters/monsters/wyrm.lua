local mtype = Game.createMonsterType("Wyrm")
local monster = {}

monster.name = "Wyrm"
monster.description = "a wyrm"
monster.experience = 1550
monster.race = "blood"
monster.maxHealth = 1825
monster.health = 1825
monster.speed = 280
monster.manaCost = 0
monster.corpse = 8941
monster.outfit = { lookType = 291 }
monster.runHealth = 150
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -235,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        minDamage = -100,
        maxDamage = -220,
        radius = 3,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -200,
        length = 5,
        spread = 2,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -100,
        maxDamage = -125,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -98,
        maxDamage = -145,
        length = 4,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 34,
    armor = 34,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 100,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "effect",
        interval = 2000,
        chance = 10,
        radius = 1,
        effect = CONST_ME_SOUND_YELLOW,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 75},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "lifedrain", combat = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GRRR", yell = false},
    {text = "GRROARR", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 96901,
        maxCount = 232,
    },
    {
        id = 2672,
        chance = 34875,
    },
    {
        id = 7588,
        chance = 20224,
    },
    {
        id = 7589,
        chance = 15215,
    },
    {
        id = 10582,
        chance = 14967,
    },
    {
        id = 2546,
        chance = 7941,
        maxCount = 10,
    },
    {
        id = 2455,
        chance = 6047,
    },
    {
        id = 8871,
        chance = 1230,
    },
    {
        id = 8921,
        chance = 1009,
    },
    {
        id = 2145,
        chance = 912,
        maxCount = 3,
    },
    {
        id = 7889,
        chance = 754,
    },
    {
        id = 8920,
        chance = 464,
    },
    {
        id = 8873,
        chance = 310,
    },
    {
        id = 8855,
        chance = 106,
    },
    {
        id = 7430,
        chance = 96,
    },
    {
        id = 10221,
        chance = 85,
    },
}

mtype:register(monster)
