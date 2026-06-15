local mtype = Game.createMonsterType("Quara Pincher")
local monster = {}

monster.name = "Quara Pincher"
monster.description = "a quara pincher"
monster.experience = 1200
monster.race = "blood"
monster.maxHealth = 1800
monster.health = 1800
monster.speed = 396
monster.manaCost = 0
monster.corpse = 6063
monster.outfit = { lookType = 77 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -340,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 1,
        speed = -600,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 85,
    armor = 85,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Clank clank!", yell = false},
    {text = "Clap!", yell = false},
    {text = "Crrrk! Crrrk!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 150,
    },
    {
        id = 2152,
        chance = 40655,
    },
    {
        id = 12446,
        chance = 14711,
    },
    {
        id = 7591,
        chance = 10371,
    },
    {
        id = 2147,
        chance = 7539,
        maxCount = 2,
    },
    {
        id = 2381,
        chance = 6979,
    },
    {
        id = 2670,
        chance = 4919,
    },
    {
        id = 5895,
        chance = 1616,
    },
    {
        id = 2475,
        chance = 1481,
    },
    {
        id = 2487,
        chance = 270,
    },
    {
        id = 7897,
        chance = 116,
    },
    {
        id = 13305,
        chance = 45,
    },
}

mtype:register(monster)
