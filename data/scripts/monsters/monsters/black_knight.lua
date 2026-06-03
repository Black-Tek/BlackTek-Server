local mtype = Game.createMonsterType("Black Knight")
local monster = {}

monster.name = "Black Knight"
monster.description = "a black knight"
monster.experience = 1600
monster.race = "blood"
monster.maxHealth = 1800
monster.health = 1800
monster.speed = 250
monster.manaCost = 0
monster.corpse = 20355
monster.outfit = {
    lookType = 131,
    lookHead = 95,
    lookBody = 95,
    lookLegs = 95,
    lookFeet = 95,
    lookAddons = 3,
}
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
    ignoreSpawnBlock = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 42,
    armor = 42,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 95},
    {type = COMBAT_ENERGYDAMAGE, percent = 80},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -8},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "MINE!", yell = false},
    {text = "NO PRISONERS!", yell = false},
    {text = "NO MERCY!", yell = false},
    {text = "By Bolg's blood!", yell = false},
    {text = "You're no match for me!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 48586,
        maxCount = 143,
    },
    {
        id = 2389,
        chance = 29429,
        maxCount = 3,
    },
    {
        id = 2691,
        chance = 20600,
        maxCount = 2,
    },
    {
        id = 2120,
        chance = 14426,
    },
    {
        id = 2478,
        chance = 13070,
    },
    {
        id = 2381,
        chance = 12522,
    },
    {
        id = 2463,
        chance = 10935,
    },
    {
        id = 3351,
        chance = 10531,
    },
    {
        id = 2377,
        chance = 9608,
    },
    {
        id = 2417,
        chance = 6896,
    },
    {
        id = 2475,
        chance = 4703,
    },
    {
        id = 2430,
        chance = 2654,
    },
    {
        id = 2490,
        chance = 2424,
    },
    {
        id = 2489,
        chance = 2020,
    },
    {
        id = 2477,
        chance = 1039,
    },
    {
        id = 2476,
        chance = 750,
    },
    {
        id = 2133,
        chance = 750,
    },
    {
        id = 7895,
        chance = 577,
    },
    {
        id = 2195,
        chance = 404,
    },
    {
        id = 2414,
        chance = 289,
    },
    {
        id = 2114,
        chance = 115,
    },
}

mtype:register(monster)
