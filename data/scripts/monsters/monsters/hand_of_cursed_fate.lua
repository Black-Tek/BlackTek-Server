local mtype = Game.createMonsterType("Hand Of Cursed Fate")
local monster = {}

monster.name = "Hand Of Cursed Fate"
monster.description = "a hand of cursed fate"
monster.experience = 5000
monster.race = "blood"
monster.maxHealth = 7500
monster.health = 7500
monster.speed = 260
monster.manaCost = 0
monster.corpse = 6312
monster.outfit = { lookType = 230 }
monster.runHealth = 3500
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 20
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
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
        maxDamage = -520,
        -- NOTE: melee condition (poison=380); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 380000,
        },
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = 0,
        maxDamage = -920,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        radius = 4,
        target = false,
        duration = 3000,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -220,
        maxDamage = -880,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 53,
    armor = 53,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 1000,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 8000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 100,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 267,
    },
    {
        id = 2152,
        chance = 98310,
        maxCount = 7,
    },
    {
        id = 5944,
        chance = 30516,
    },
    {
        id = 6558,
        chance = 29935,
    },
    {
        id = 7590,
        chance = 19632,
        maxCount = 2,
    },
    {
        id = 8473,
        chance = 18835,
    },
    {
        id = 2178,
        chance = 15373,
    },
    {
        id = 7368,
        chance = 15116,
        maxCount = 5,
    },
    {
        id = 2146,
        chance = 14413,
        maxCount = 4,
    },
    {
        id = 6500,
        chance = 11696,
    },
    {
        id = 2200,
        chance = 10249,
    },
    {
        id = 2154,
        chance = 5773,
    },
    {
        id = 2127,
        chance = 5219,
    },
    {
        id = 2187,
        chance = 4800,
    },
    {
        id = 2268,
        chance = 4381,
        maxCount = 8,
    },
    {
        id = 2476,
        chance = 3921,
    },
    {
        id = 2167,
        chance = 2177,
    },
    {
        id = 2158,
        chance = 1690,
    },
    {
        id = 6300,
        chance = 1312,
    },
    {
        id = 2153,
        chance = 1001,
    },
    {
        id = 2171,
        chance = 960,
    },
    {
        id = 2436,
        chance = 933,
    },
    {
        id = 1962,
        chance = 879,
    },
    {
        id = 2487,
        chance = 879,
    },
    {
        id = 9971,
        chance = 568,
    },
    {
        id = 2195,
        chance = 135,
    },
    {
        id = 5669,
        chance = 108,
    },
    {
        id = 7414,
        chance = 68,
    },
}

mtype:register(monster)
