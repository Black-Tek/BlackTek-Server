local mtype = Game.createMonsterType("Hero")
local monster = {}

monster.name = "Hero"
monster.description = "a hero"
monster.experience = 1200
monster.race = "blood"
monster.maxHealth = 1400
monster.health = 1400
monster.speed = 280
monster.manaCost = 0
monster.corpse = 20415
monster.outfit = { lookType = 73 }
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
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -240,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -120,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Let's have a fight!", yell = false},
    {text = "Welcome to my battleground!", yell = false},
    {text = "Have you seen princess Lumelia?", yell = false},
    {text = "I will sing a tune at your grave.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 60324,
        maxCount = 100,
    },
    {
        id = 1949,
        chance = 45128,
    },
    {
        id = 2544,
        chance = 27005,
        maxCount = 13,
    },
    {
        id = 2744,
        chance = 20384,
    },
    {
        id = 2681,
        chance = 20164,
    },
    {
        id = 2456,
        chance = 13005,
    },
    {
        id = 7364,
        chance = 11558,
        maxCount = 4,
    },
    {
        id = 2666,
        chance = 8585,
    },
    {
        id = 2652,
        chance = 7964,
    },
    {
        id = 2121,
        chance = 5153,
    },
    {
        id = 12466,
        chance = 5038,
    },
    {
        id = 2120,
        chance = 2079,
    },
    {
        id = 5911,
        chance = 2013,
    },
    {
        id = 2377,
        chance = 1612,
    },
    {
        id = 2071,
        chance = 1577,
    },
    {
        id = 2661,
        chance = 1120,
    },
    {
        id = 2391,
        chance = 959,
    },
    {
        id = 12406,
        chance = 952,
    },
    {
        id = 7591,
        chance = 736,
    },
    {
        id = 2487,
        chance = 614,
    },
    {
        id = 2488,
        chance = 586,
    },
    {
        id = 2164,
        chance = 527,
    },
    {
        id = 2392,
        chance = 478,
    },
    {
        id = 2491,
        chance = 450,
    },
    {
        id = 2519,
        chance = 202,
    },
    {
        id = 2114,
        chance = 73,
    },
}

mtype:register(monster)
