local mtype = Game.createMonsterType("Worker Golem")
local monster = {}

monster.name = "Worker Golem"
monster.description = "a worker golem"
monster.experience = 1250
monster.race = "energy"
monster.maxHealth = 1470
monster.health = 1470
monster.speed = 160
monster.manaCost = 0
monster.corpse = 9801
monster.outfit = { lookType = 304 }
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
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -125,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "INTRUDER ALARM!", yell = false},
    {text = "klonk klonk klonk", yell = false},
    {text = "Rrrtttarrrttarrrtta", yell = false},
    {text = "Awaiting orders.", yell = false},
    {text = "Secret objective complete.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 93817,
        maxCount = 139,
    },
    {
        id = 8309,
        chance = 4809,
        maxCount = 5,
    },
    {
        id = 10572,
        chance = 2169,
    },
    {
        id = 7591,
        chance = 1974,
    },
    {
        id = 7590,
        chance = 1574,
    },
    {
        id = 9809,
        chance = 1559,
    },
    {
        id = 9690,
        chance = 1066,
    },
    {
        id = 2177,
        chance = 1010,
    },
    {
        id = 2391,
        chance = 990,
    },
    {
        id = 2145,
        chance = 974,
        maxCount = 2,
    },
    {
        id = 9812,
        chance = 928,
    },
    {
        id = 7452,
        chance = 913,
    },
    {
        id = 5880,
        chance = 872,
    },
    {
        id = 8472,
        chance = 820,
    },
    {
        id = 7439,
        chance = 784,
    },
    {
        id = 2164,
        chance = 502,
    },
    {
        id = 9979,
        chance = 159,
    },
    {
        id = 7428,
        chance = 113,
    },
}

mtype:register(monster)
