local mtype = Game.createMonsterType("Orc")
local monster = {}

monster.name = "Orc"
monster.description = "an orc"
monster.experience = 25
monster.race = "blood"
monster.maxHealth = 70
monster.health = 70
monster.speed = 150
monster.manaCost = 300
monster.corpse = 5966
monster.outfit = { lookType = 5 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
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
        maxDamage = -35,
    },
}
monster.defenses = {
    defense = 4,
    armor = 4,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grak brrretz!", yell = false},
    {text = "Grow truk grrrrr.", yell = false},
    {text = "Prek tars, dekklep zurk.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 84724,
        maxCount = 14,
    },
    {
        id = 2666,
        chance = 9613,
    },
    {
        id = 2484,
        chance = 8232,
    },
    {
        id = 2526,
        chance = 6685,
    },
    {
        id = 2385,
        chance = 5580,
    },
    {
        id = 2386,
        chance = 5442,
    },
    {
        id = 2482,
        chance = 2873,
    },
    {
        id = 12435,
        chance = 359,
    },
    {
        id = 11113,
        chance = 55,
    },
}

mtype:register(monster)
