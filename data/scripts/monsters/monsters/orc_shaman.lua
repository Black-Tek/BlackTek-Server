local mtype = Game.createMonsterType("Orc Shaman")
local monster = {}

monster.name = "Orc Shaman"
monster.description = "an orc shaman"
monster.experience = 110
monster.race = "blood"
monster.maxHealth = 115
monster.health = 115
monster.speed = 140
monster.manaCost = 0
monster.corpse = 5978
monster.outfit = { lookType = 6 }
monster.runHealth = 15
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
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -15,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -20,
        maxDamage = -35,
        shootEffect = CONST_ANI_ENERGYBALL,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -5,
        maxDamage = -45,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
    {
        name = "healing",
        interval = 2000,
        chance = 60,
        minDamage = 27,
        maxDamage = 43,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Snake", interval = 2000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Huumans stinkk!", yell = false},
    {text = "Grak brrretz gulu.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 89750,
        maxCount = 5,
    },
    {
        id = 2686,
        chance = 10877,
        maxCount = 2,
    },
    {
        id = 12408,
        chance = 10161,
    },
    {
        id = 2464,
        chance = 8658,
    },
    {
        id = 12434,
        chance = 7513,
    },
    {
        id = 2389,
        chance = 5170,
    },
    {
        id = 12435,
        chance = 3792,
    },
    {
        id = 11113,
        chance = 2182,
    },
    {
        id = 2188,
        chance = 966,
    },
    {
        id = 1958,
        chance = 465,
    },
}

mtype:register(monster)
