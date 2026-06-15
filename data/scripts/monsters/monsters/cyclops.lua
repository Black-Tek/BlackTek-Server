local mtype = Game.createMonsterType("Cyclops")
local monster = {}

monster.name = "Cyclops"
monster.description = "a cyclops"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 260
monster.health = 260
monster.speed = 190
monster.manaCost = 490
monster.corpse = 5962
monster.outfit = { lookType = 22 }
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
}
monster.defenses = {
    defense = 17,
    armor = 17,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Il lorstok human!", yell = false},
    {text = "Toks utat.", yell = false},
    {text = "Human, uh whil dyh!", yell = false},
    {text = "Youh ah trak!", yell = false},
    {text = "Let da mashing begin!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82276,
        maxCount = 47,
    },
    {
        id = 2666,
        chance = 29087,
    },
    {
        id = 2406,
        chance = 7795,
    },
    {
        id = 10574,
        chance = 5265,
    },
    {
        id = 2510,
        chance = 2442,
    },
    {
        id = 2513,
        chance = 1462,
    },
    {
        id = 2381,
        chance = 892,
    },
    {
        id = 2129,
        chance = 322,
    },
    {
        id = 2490,
        chance = 234,
    },
    {
        id = 7618,
        chance = 146,
    },
    {
        id = 7398,
        chance = 117,
    },
    {
        id = 2209,
        chance = 44,
    },
}

mtype:register(monster)
