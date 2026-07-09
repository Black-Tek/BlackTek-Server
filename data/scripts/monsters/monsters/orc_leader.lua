local mtype = Game.createMonsterType("Orc Leader")
local monster = {}

monster.name = "Orc Leader"
monster.description = "an orc leader"
monster.experience = 270
monster.race = "blood"
monster.maxHealth = 450
monster.health = 450
monster.speed = 230
monster.manaCost = 640
monster.corpse = 6001
monster.outfit = { lookType = 59 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = true,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -185,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -70,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ulderek futgyr human!", yell = false},
}
monster.loot = {
    {
        id = 2667,
        chance = 30075,
    },
    {
        id = 2148,
        chance = 28207,
        maxCount = 35,
    },
    {
        id = 12435,
        chance = 19721,
    },
    {
        id = 2510,
        chance = 10140,
    },
    {
        id = 2410,
        chance = 10007,
        maxCount = 4,
    },
    {
        id = 2789,
        chance = 9833,
    },
    {
        id = 2207,
        chance = 3778,
    },
    {
        id = 2397,
        chance = 2970,
    },
    {
        id = 7378,
        chance = 2647,
    },
    {
        id = 2478,
        chance = 2435,
    },
    {
        id = 2419,
        chance = 2191,
    },
    {
        id = 12436,
        chance = 2115,
    },
    {
        id = 2463,
        chance = 1597,
    },
    {
        id = 11113,
        chance = 1022,
    },
    {
        id = 2413,
        chance = 748,
    },
    {
        id = 7618,
        chance = 583,
    },
    {
        id = 2647,
        chance = 396,
    },
    {
        id = 2475,
        chance = 114,
    },
}

mtype:register(monster)
