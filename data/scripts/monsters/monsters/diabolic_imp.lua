local mtype = Game.createMonsterType("Diabolic Imp")
local monster = {}

monster.name = "Diabolic Imp"
monster.description = "a diabolic imp"
monster.experience = 2900
monster.race = "fire"
monster.maxHealth = 1950
monster.health = 1950
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6364
monster.outfit = { lookType = 237 }
monster.runHealth = 400
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -240,
        -- NOTE: melee condition (poison=160); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 160000,
        },
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -100,
        maxDamage = -240,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -300,
        maxDamage = -430,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "diabolic imp skill reducer",
        interval = 2000,
        chance = 5,
        range = 5,
    },
}
monster.defenses = {
    defense = 29,
    armor = 29,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 650,
        maxDamage = 800,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 800,
        duration = 2000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 1000,
        effect = CONST_ME_TELEPORT,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Muahaha!", yell = false},
    {text = "He he he.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99347,
        maxCount = 200,
    },
    {
        id = 2548,
        chance = 50468,
    },
    {
        id = 6558,
        chance = 24610,
    },
    {
        id = 2260,
        chance = 14973,
        maxCount = 2,
    },
    {
        id = 2568,
        chance = 8828,
    },
    {
        id = 2515,
        chance = 8274,
    },
    {
        id = 6500,
        chance = 7777,
    },
    {
        id = 5944,
        chance = 6912,
    },
    {
        id = 2419,
        chance = 5351,
    },
    {
        id = 2152,
        chance = 3364,
        maxCount = 7,
    },
    {
        id = 2165,
        chance = 2739,
    },
    {
        id = 2150,
        chance = 2583,
        maxCount = 3,
    },
    {
        id = 2387,
        chance = 1746,
    },
    {
        id = 2185,
        chance = 781,
    },
    {
        id = 7900,
        chance = 525,
    },
    {
        id = 7899,
        chance = 355,
    },
    {
        id = 6300,
        chance = 99,
    },
}

mtype:register(monster)
