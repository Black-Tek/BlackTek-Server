local mtype = Game.createMonsterType("Stonecracker")
local monster = {}

monster.name = "Stonecracker"
monster.description = "Stonecracker"
monster.experience = 3500
monster.race = "blood"
monster.maxHealth = 6500
monster.health = 6500
monster.speed = 280
monster.manaCost = 0
monster.corpse = 5999
monster.outfit = { lookType = 55 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canWalkOnEnergy = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -500,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -200,
        maxDamage = -280,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 45,
    armor = 40,
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        speed = 360,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 500,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 85},
    {type = COMBAT_HOLYDAMAGE, percent = 35},
    {type = COMBAT_FIREDAMAGE, percent = 40},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "HUAHAHA!", yell = false},
}
monster.loot = {
    {
        id = 5930,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 162,
    },
    {
        id = 5893,
        chance = 100000,
    },
    {
        id = 7396,
        chance = 97619,
    },
    {
        id = 2150,
        chance = 69048,
        maxCount = 5,
    },
    {
        id = 7368,
        chance = 64286,
        maxCount = 94,
    },
    {
        id = 2377,
        chance = 52381,
    },
    {
        id = 2666,
        chance = 23810,
    },
    {
        id = 2489,
        chance = 14286,
    },
    {
        id = 2387,
        chance = 7143,
    },
    {
        id = 11227,
        chance = 4762,
    },
}

mtype:register(monster)
