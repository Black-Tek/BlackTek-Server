local mtype = Game.createMonsterType("Dworc Venomsniper")
local monster = {}

monster.name = "Dworc Venomsniper"
monster.experience = 35
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 152
monster.manaCost = 0
monster.corpse = 6059
monster.outfit = { lookType = 216 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -15,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 5,
        minDamage = -20,
        maxDamage = -40,
    },
}
monster.defenses = {
    defense = 3,
    armor = 3,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = -13},
    {type = COMBAT_DEATHDAMAGE, percent = -8},
    {type = COMBAT_ICEDAMAGE, percent = -13},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Brak brrretz!", yell = false},
    {text = "Grow truk grrrrr.", yell = false},
    {text = "Prek tars, dekklep zurk.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 74992,
        maxCount = 13,
    },
    {
        id = 2467,
        chance = 10048,
    },
    {
        id = 2410,
        chance = 7906,
        maxCount = 2,
    },
    {
        id = 2050,
        chance = 5486,
    },
    {
        id = 2545,
        chance = 4957,
        maxCount = 3,
    },
    {
        id = 2411,
        chance = 1500,
    },
    {
        id = 2229,
        chance = 983,
        maxCount = 2,
    },
    {
        id = 3967,
        chance = 502,
    },
    {
        id = 7732,
        chance = 193,
    },
    {
        id = 3983,
        chance = 99,
    },
    {
        id = 2172,
        chance = 97,
    },
}

mtype:register(monster)
