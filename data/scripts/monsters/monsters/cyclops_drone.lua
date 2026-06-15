local mtype = Game.createMonsterType("Cyclops Drone")
local monster = {}

monster.name = "Cyclops Drone"
monster.description = "a cyclops drone"
monster.experience = 200
monster.race = "blood"
monster.maxHealth = 325
monster.health = 325
monster.speed = 198
monster.manaCost = 525
monster.corpse = 7847
monster.outfit = { lookType = 280 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
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
        maxDamage = -105,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 35,
        range = 7,
        minDamage = 0,
        maxDamage = -80,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 1},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Fee! Fie! Foe! Fum!", yell = false},
    {text = "Luttl pest!", yell = false},
    {text = "Me makking you pulp!", yell = false},
    {text = "Humy tasy! Hum hum!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82050,
        maxCount = 30,
    },
    {
        id = 2666,
        chance = 50295,
    },
    {
        id = 2406,
        chance = 7867,
    },
    {
        id = 10574,
        chance = 6895,
    },
    {
        id = 2510,
        chance = 2026,
    },
    {
        id = 2513,
        chance = 1580,
    },
    {
        id = 2381,
        chance = 679,
    },
    {
        id = 7588,
        chance = 517,
    },
    {
        id = 2490,
        chance = 198,
    },
    {
        id = 7398,
        chance = 124,
    },
    {
        id = 2209,
        chance = 73,
    },
}

mtype:register(monster)
