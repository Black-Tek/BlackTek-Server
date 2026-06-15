local mtype = Game.createMonsterType("Orewalker")
local monster = {}

monster.name = "Orewalker"
monster.description = "an orewalker"
monster.experience = 4800
monster.race = "undead"
monster.maxHealth = 7200
monster.health = 7200
monster.speed = 380
monster.manaCost = 0
monster.corpse = 17256
monster.outfit = { lookType = 490 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        minDamage = -296,
        maxDamage = -700,
        length = 4,
        spread = 3,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -1500,
        length = 6,
        target = false,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -800,
        maxDamage = -1080,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 15,
        radius = 4,
        target = false,
        duration = 6000,
        effect = CONST_ME_SOUND_PURPLE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 2,
        target = false,
        speed = -800,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 79,
    armor = 79,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 25},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = 65},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 25},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Sizzle!", yell = false},
    {text = "CLONK!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 198,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 10,
    },
    {
        id = 11232,
        chance = 20887,
    },
    {
        id = 18427,
        chance = 20088,
    },
    {
        id = 5880,
        chance = 16134,
    },
    {
        id = 7588,
        chance = 15615,
        maxCount = 2,
    },
    {
        id = 9970,
        chance = 15136,
        maxCount = 3,
    },
    {
        id = 7620,
        chance = 15056,
        maxCount = 4,
    },
    {
        id = 18435,
        chance = 14936,
        maxCount = 5,
    },
    {
        id = 7590,
        chance = 14337,
        maxCount = 2,
    },
    {
        id = 18429,
        chance = 14297,
    },
    {
        id = 7589,
        chance = 14257,
        maxCount = 2,
    },
    {
        id = 18418,
        chance = 13898,
        maxCount = 2,
    },
    {
        id = 18419,
        chance = 13099,
    },
    {
        id = 11227,
        chance = 12660,
    },
    {
        id = 8473,
        chance = 9265,
        maxCount = 2,
    },
    {
        id = 18415,
        chance = 8107,
    },
    {
        id = 2213,
        chance = 4273,
    },
    {
        id = 7413,
        chance = 2756,
    },
    {
        id = 7454,
        chance = 2716,
    },
    {
        id = 5904,
        chance = 2716,
    },
    {
        id = 2477,
        chance = 1877,
    },
    {
        id = 18390,
        chance = 1398,
    },
    {
        id = 2154,
        chance = 998,
    },
    {
        id = 2491,
        chance = 958,
    },
    {
        id = 2487,
        chance = 359,
    },
    {
        id = 8878,
        chance = 359,
    },
    {
        id = 18453,
        chance = 240,
    },
}

mtype:register(monster)
