local mtype = Game.createMonsterType("Behemoth")
local monster = {}

monster.name = "Behemoth"
monster.description = "a behemoth"
monster.experience = 2500
monster.race = "blood"
monster.maxHealth = 4000
monster.health = 4000
monster.speed = 340
monster.manaCost = 0
monster.corpse = 5999
monster.outfit = { lookType = 55 }
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
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -455,
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
    defense = 50,
    armor = 50,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 390,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You're so little!", yell = false},
    {text = "Human flesh - delicious!", yell = false},
    {text = "Crush the intruders!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 200,
    },
    {
        id = 2152,
        chance = 60256,
        maxCount = 5,
    },
    {
        id = 2666,
        chance = 29641,
    },
    {
        id = 12403,
        chance = 14968,
    },
    {
        id = 2387,
        chance = 10039,
    },
    {
        id = 7368,
        chance = 9988,
        maxCount = 5,
    },
    {
        id = 2150,
        chance = 6054,
        maxCount = 5,
    },
    {
        id = 2377,
        chance = 6019,
    },
    {
        id = 7591,
        chance = 5072,
    },
    {
        id = 2489,
        chance = 4174,
    },
    {
        id = 2463,
        chance = 4110,
    },
    {
        id = 2125,
        chance = 2549,
    },
    {
        id = 5893,
        chance = 999,
    },
    {
        id = 2393,
        chance = 966,
    },
    {
        id = 2174,
        chance = 830,
    },
    {
        id = 2231,
        chance = 691,
    },
    {
        id = 3456,
        chance = 619,
    },
    {
        id = 5930,
        chance = 469,
    },
    {
        id = 2645,
        chance = 434,
    },
    {
        id = 2416,
        chance = 128,
    },
    {
        id = 7396,
        chance = 114,
    },
    {
        id = 2023,
        chance = 97,
    },
    {
        id = 7413,
        chance = 75,
    },
    {
        id = 2454,
        chance = 70,
    },
}

mtype:register(monster)
