local mtype = Game.createMonsterType("Drillworm")
local monster = {}

monster.name = "Drillworm"
monster.description = "a drillworm"
monster.experience = 858
monster.race = "blood"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 220
monster.manaCost = 0
monster.corpse = 19705
monster.outfit = { lookType = 527 }
monster.changeTarget = {
    interval = 4000,
    chance = 20,
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
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        length = 8,
        target = false,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -150,
        maxDamage = -300,
        length = 8,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -100,
        maxDamage = -150,
        radius = 3,
        target = true,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 41,
    armor = 41,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 16},
    {type = COMBAT_HOLYDAMAGE, percent = 25},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Krrrk!", yell = false},
    {text = "Knarrrk!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 150,
    },
    {
        id = 11222,
        chance = 15049,
    },
    {
        id = 13757,
        chance = 12155,
    },
    {
        id = 18417,
        chance = 10095,
    },
    {
        id = 18416,
        chance = 10029,
    },
    {
        id = 18418,
        chance = 9978,
    },
    {
        id = 18429,
        chance = 7329,
    },
    {
        id = 18427,
        chance = 5172,
    },
    {
        id = 3456,
        chance = 5087,
    },
    {
        id = 18436,
        chance = 5000,
        maxCount = 2,
    },
    {
        id = 3976,
        chance = 5000,
        maxCount = 5,
    },
    {
        id = 7887,
        chance = 2571,
    },
    {
        id = 2213,
        chance = 2387,
    },
    {
        id = 5880,
        chance = 1537,
    },
    {
        id = 11339,
        chance = 751,
    },
    {
        id = 7452,
        chance = 519,
    },
}

mtype:register(monster)
