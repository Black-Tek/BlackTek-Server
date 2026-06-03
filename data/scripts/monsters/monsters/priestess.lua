local mtype = Game.createMonsterType("Priestess")
local monster = {}

monster.name = "Priestess"
monster.description = "a priestess"
monster.experience = 420
monster.race = "blood"
monster.maxHealth = 390
monster.health = 390
monster.speed = 170
monster.manaCost = 0
monster.corpse = 20491
monster.outfit = { lookType = 58 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -75,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -55,
        maxDamage = -120,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -2,
        maxDamage = -170,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 7,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 40},
    {type = COMBAT_EARTHDAMAGE, percent = 70},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "ghoul", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Your energy is mine.", yell = false},
    {text = "Now your life is come to the end, hahahaha!", yell = false},
    {text = "Throw the soul on the altar!", yell = false},
}
monster.loot = {
    {
        id = 2802,
        chance = 13494,
    },
    {
        id = 2760,
        chance = 12093,
    },
    {
        id = 11220,
        chance = 9795,
    },
    {
        id = 2674,
        chance = 7403,
        maxCount = 2,
    },
    {
        id = 2803,
        chance = 6183,
    },
    {
        id = 10562,
        chance = 5036,
    },
    {
        id = 2791,
        chance = 3343,
    },
    {
        id = 10556,
        chance = 1796,
    },
    {
        id = 2423,
        chance = 1484,
    },
    {
        id = 2070,
        chance = 1425,
    },
    {
        id = 2192,
        chance = 1132,
    },
    {
        id = 2183,
        chance = 1069,
    },
    {
        id = 1962,
        chance = 932,
    },
    {
        id = 7620,
        chance = 932,
    },
    {
        id = 2151,
        chance = 717,
    },
    {
        id = 2125,
        chance = 615,
    },
    {
        id = 2529,
        chance = 210,
    },
    {
        id = 2114,
        chance = 68,
    },
}

mtype:register(monster)
