local mtype = Game.createMonsterType("Apprentice Sheng")
local monster = {}

monster.name = "Apprentice Sheng"
monster.description = "Apprentice Sheng"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 95
monster.health = 95
monster.speed = 170
monster.manaCost = 0
monster.corpse = 5981
monster.outfit = { lookType = 23 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -10,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 14,
        range = 7,
        minDamage = -15,
        maxDamage = -25,
        shootEffect = CONST_ANI_ENERGYBALL,
        effect = CONST_ME_ENERGYAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -15,
        maxDamage = -45,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "energyfield",
        interval = 1000,
        chance = 8,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "effect",
        interval = 1050,
        chance = 100,
        radius = 3,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 18,
    armor = 18,
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Hyaena", interval = 1050, chance = 100, max = 0},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "I will protect the secrets of my master!", yell = false},
    {text = "Kaplar!", yell = false},
    {text = "This isle will become ours alone", yell = false},
    {text = "You already know too much.", yell = false},
}
monster.loot = {
    {
        id = 5878,
        chance = 100000,
    },
    {
        id = 2162,
        chance = 82222,
    },
    {
        id = 2148,
        chance = 71111,
        maxCount = 10,
    },
    {
        id = 2050,
        chance = 24444,
        maxCount = 2,
    },
    {
        id = 2649,
        chance = 13333,
    },
    {
        id = 2403,
        chance = 11111,
    },
    {
        id = 2684,
        chance = 6667,
        maxCount = 7,
    },
    {
        id = 2461,
        chance = 6667,
    },
}

mtype:register(monster)
