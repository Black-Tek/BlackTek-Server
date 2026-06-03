local mtype = Game.createMonsterType("Orc Rider")
local monster = {}

monster.name = "Orc Rider"
monster.description = "an orc rider"
monster.experience = 110
monster.race = "blood"
monster.maxHealth = 180
monster.health = 180
monster.speed = 260
monster.manaCost = 490
monster.corpse = 6010
monster.outfit = { lookType = 4 }
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
        maxDamage = -130,
    },
}
monster.defenses = {
    defense = 9,
    armor = 9,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 290,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
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
    {text = "Grrrrrrr", yell = false},
    {text = "Orc arga Huummmak!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 48261,
        maxCount = 81,
    },
    {
        id = 2666,
        chance = 24151,
    },
    {
        id = 2129,
        chance = 10159,
    },
    {
        id = 12435,
        chance = 9984,
    },
    {
        id = 11235,
        chance = 9718,
    },
    {
        id = 2428,
        chance = 6969,
        maxCount = 2,
    },
    {
        id = 11113,
        chance = 2110,
    },
    {
        id = 2050,
        chance = 1049,
    },
    {
        id = 2513,
        chance = 1019,
    },
    {
        id = 2425,
        chance = 1007,
    },
    {
        id = 2483,
        chance = 615,
    },
}

mtype:register(monster)
