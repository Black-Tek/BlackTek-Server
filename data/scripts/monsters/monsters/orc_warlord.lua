local mtype = Game.createMonsterType("Orc Warlord")
local monster = {}

monster.name = "Orc Warlord"
monster.description = "an orc warlord"
monster.experience = 670
monster.race = "blood"
monster.maxHealth = 950
monster.health = 950
monster.speed = 234
monster.manaCost = 0
monster.corpse = 6008
monster.outfit = { lookType = 2 }
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
        maxDamage = -250,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
}
monster.defenses = {
    defense = 28,
    armor = 28,
    {
        name = "invisible",
        interval = 2000,
        chance = 5,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ranat Ulderek!", yell = false},
    {text = "Orc buta bana!", yell = false},
    {text = "Ikem rambo zambo!", yell = false},
    {text = "Futchi maruk buta!", yell = false},
}
monster.loot = {
    {
        id = 12409,
        chance = 25259,
    },
    {
        id = 12435,
        chance = 20351,
    },
    {
        id = 2148,
        chance = 18998,
        maxCount = 45,
    },
    {
        id = 2399,
        chance = 14658,
        maxCount = 18,
    },
    {
        id = 2667,
        chance = 10484,
        maxCount = 2,
    },
    {
        id = 11113,
        chance = 10033,
    },
    {
        id = 2463,
        chance = 5526,
    },
    {
        id = 2428,
        chance = 5426,
    },
    {
        id = 12436,
        chance = 4958,
    },
    {
        id = 3965,
        chance = 4825,
    },
    {
        id = 2647,
        chance = 3907,
    },
    {
        id = 2419,
        chance = 2972,
    },
    {
        id = 2377,
        chance = 1953,
    },
    {
        id = 2200,
        chance = 1853,
    },
    {
        id = 2490,
        chance = 1285,
    },
    {
        id = 2465,
        chance = 935,
    },
    {
        id = 7618,
        chance = 384,
    },
    {
        id = 2497,
        chance = 301,
    },
    {
        id = 2434,
        chance = 234,
    },
    {
        id = 7891,
        chance = 184,
    },
    {
        id = 7395,
        chance = 134,
    },
    {
        id = 2165,
        chance = 117,
    },
}

mtype:register(monster)
