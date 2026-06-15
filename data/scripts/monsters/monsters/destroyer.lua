local mtype = Game.createMonsterType("Destroyer")
local monster = {}

monster.name = "Destroyer"
monster.description = "a destroyer"
monster.experience = 2500
monster.race = "blood"
monster.maxHealth = 3700
monster.health = 3700
monster.speed = 300
monster.manaCost = 0
monster.corpse = 6320
monster.outfit = { lookType = 236 }
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -500,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 420,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = -15},
    {type = COMBAT_HOLYDAMAGE, percent = -3},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "COME HERE AND DIE!", yell = false},
    {text = "Destructiooooon!", yell = false},
    {text = "It's a good day to destroy!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 92584,
        maxCount = 282,
    },
    {
        id = 2666,
        chance = 39280,
    },
    {
        id = 6500,
        chance = 13803,
    },
    {
        id = 2416,
        chance = 10573,
    },
    {
        id = 2546,
        chance = 8590,
        maxCount = 12,
    },
    {
        id = 2489,
        chance = 5947,
    },
    {
        id = 2150,
        chance = 5947,
        maxCount = 2,
    },
    {
        id = 5944,
        chance = 5433,
    },
    {
        id = 3456,
        chance = 4699,
    },
    {
        id = 11215,
        chance = 4552,
    },
    {
        id = 2463,
        chance = 3304,
    },
    {
        id = 2152,
        chance = 2203,
        maxCount = 3,
    },
    {
        id = 2393,
        chance = 1248,
    },
    {
        id = 7591,
        chance = 1101,
    },
    {
        id = 7427,
        chance = 808,
    },
    {
        id = 2125,
        chance = 514,
    },
    {
        id = 7419,
        chance = 294,
    },
    {
        id = 2645,
        chance = 294,
    },
    {
        id = 6300,
        chance = 220,
    },
    {
        id = 2178,
        chance = 73,
    },
}

mtype:register(monster)
