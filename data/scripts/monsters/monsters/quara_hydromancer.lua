local mtype = Game.createMonsterType("Quara Hydromancer")
local monster = {}

monster.name = "Quara Hydromancer"
monster.description = "a quara hydromancer"
monster.experience = 800
monster.race = "blood"
monster.maxHealth = 1100
monster.health = 1100
monster.speed = 490
monster.manaCost = 0
monster.corpse = 6066
monster.outfit = { lookType = 47 }
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
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -180,
        length = 8,
        effect = CONST_ME_BUBBLES,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -90,
        maxDamage = -150,
        radius = 3,
        target = false,
        effect = CONST_ME_BUBBLES,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -170,
        maxDamage = -240,
        length = 8,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -170,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 100,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Qua hah tsh!", yell = false},
    {text = "Teech tsha tshul!", yell = false},
    {text = "Quara tsha Fach!", yell = false},
    {text = "Tssssha Quara!", yell = false},
    {text = "Blubber.", yell = false},
    {text = "Blup.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 90418,
        maxCount = 110,
    },
    {
        id = 12444,
        chance = 15606,
    },
    {
        id = 2143,
        chance = 5083,
    },
    {
        id = 2149,
        chance = 4960,
        maxCount = 2,
    },
    {
        id = 2670,
        chance = 4716,
    },
    {
        id = 2144,
        chance = 3125,
    },
    {
        id = 7590,
        chance = 2861,
    },
    {
        id = 5895,
        chance = 1459,
    },
    {
        id = 2214,
        chance = 1233,
    },
    {
        id = 2189,
        chance = 979,
    },
    {
        id = 2476,
        chance = 226,
    },
}

mtype:register(monster)
