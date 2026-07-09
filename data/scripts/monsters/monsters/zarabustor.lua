local mtype = Game.createMonsterType("Zarabustor")
local monster = {}

monster.name = "Zarabustor"
monster.description = "Zarabustor"
monster.experience = 8000
monster.race = "blood"
monster.maxHealth = 5100
monster.health = 5100
monster.speed = 440
monster.manaCost = 0
monster.corpse = 20554
monster.outfit = {
    lookType = 130,
    lookHead = 0,
    lookBody = 77,
    lookLegs = 92,
    lookFeet = 115,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -130,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -250,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_BURSTARROW,
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
        name = "physical",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = 0,
        maxDamage = -250,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -350,
        length = 8,
        effect = CONST_ME_BIGCLOUDS,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -250,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -330,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "warlock skill reducer",
        interval = 2000,
        chance = 5,
        range = 5,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
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
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Warlock", interval = 2000, chance = 10, max = 2},
    {name = "Green Djinn", interval = 2000, chance = 10, max = 3},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Killing is such a splendid diversion from my studies.", yell = false},
    {text = "Time to test my newest spells!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 32000,
        maxCount = 80,
    },
    {
        id = 2679,
        chance = 21000,
        maxCount = 4,
    },
    {
        id = 2689,
        chance = 11000,
    },
    {
        id = 2411,
        chance = 9600,
    },
    {
        id = 2436,
        chance = 8330,
    },
    {
        id = 7591,
        chance = 7190,
    },
    {
        id = 7590,
        chance = 6760,
    },
    {
        id = 7368,
        chance = 5500,
        maxCount = 4,
    },
    {
        id = 2792,
        chance = 5000,
    },
    {
        id = 2167,
        chance = 4200,
    },
    {
        id = 2178,
        chance = 4000,
    },
    {
        id = 2047,
        chance = 3500,
    },
    {
        id = 2656,
        chance = 3390,
    },
    {
        id = 2146,
        chance = 3190,
    },
    {
        id = 2151,
        chance = 3160,
    },
    {
        id = 7898,
        chance = 3040,
    },
    {
        id = 2600,
        chance = 3000,
    },
    {
        id = 2124,
        chance = 2670,
    },
    {
        id = 12410,
        chance = 2500,
    },
    {
        id = 2123,
        chance = 2420,
    },
    {
        id = 2197,
        chance = 2320,
    },
    {
        id = 1986,
        chance = 2310,
    },
    {
        id = 2466,
        chance = 2240,
    },
    {
        id = 2114,
        chance = 2060,
    },
}

mtype:register(monster)
