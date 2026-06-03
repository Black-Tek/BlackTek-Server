local mtype = Game.createMonsterType("Warlock")
local monster = {}

monster.name = "Warlock"
monster.description = "a warlock"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 3500
monster.health = 3500
monster.speed = 230
monster.manaCost = 0
monster.corpse = 20527
monster.outfit = {
    lookType = 130,
    lookHead = 0,
    lookBody = 52,
    lookLegs = 128,
    lookFeet = 95,
    lookAddons = 1,
}
monster.runHealth = 900
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -130,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -90,
        maxDamage = -180,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "warlock skill reducer",
        interval = 2000,
        chance = 5,
        range = 5,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -120,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -50,
        maxDamage = -180,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -150,
        maxDamage = -230,
        length = 8,
        effect = CONST_ME_BIGCLOUDS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -600,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 33,
    armor = 33,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 100,
        maxDamage = 225,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 20,
        duration = 10000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 95},
    {type = COMBAT_HOLYDAMAGE, percent = -8},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "stone golem", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Learn the secret of our magic! YOUR death!", yell = false},
    {text = "Even a rat is a better mage than you.", yell = false},
    {text = "We don't like intruders!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 30006,
        maxCount = 80,
    },
    {
        id = 2679,
        chance = 19630,
        maxCount = 4,
    },
    {
        id = 2689,
        chance = 8925,
    },
    {
        id = 2411,
        chance = 7693,
    },
    {
        id = 2436,
        chance = 6088,
    },
    {
        id = 7591,
        chance = 5094,
    },
    {
        id = 7590,
        chance = 5017,
    },
    {
        id = 7368,
        chance = 3289,
        maxCount = 4,
    },
    {
        id = 2792,
        chance = 2934,
    },
    {
        id = 2178,
        chance = 2128,
    },
    {
        id = 2167,
        chance = 1967,
    },
    {
        id = 2047,
        chance = 1573,
    },
    {
        id = 2656,
        chance = 1561,
    },
    {
        id = 2146,
        chance = 1283,
    },
    {
        id = 2151,
        chance = 1051,
    },
    {
        id = 7898,
        chance = 1006,
    },
    {
        id = 2600,
        chance = 935,
    },
    {
        id = 2124,
        chance = 742,
    },
    {
        id = 12410,
        chance = 477,
    },
    {
        id = 2123,
        chance = 406,
    },
    {
        id = 2197,
        chance = 355,
    },
    {
        id = 1986,
        chance = 239,
    },
    {
        id = 2466,
        chance = 206,
    },
    {
        id = 2114,
        chance = 77,
    },
}

mtype:register(monster)
