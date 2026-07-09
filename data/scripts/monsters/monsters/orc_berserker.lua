local mtype = Game.createMonsterType("Orc Berserker")
local monster = {}

monster.name = "Orc Berserker"
monster.description = "an orc berserker"
monster.experience = 195
monster.race = "blood"
monster.maxHealth = 210
monster.health = 210
monster.speed = 250
monster.manaCost = 590
monster.corpse = 5980
monster.outfit = { lookType = 8 }
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
        maxDamage = -200,
    },
}
monster.defenses = {
    defense = 12,
    armor = 12,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 290,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "KRAK ORRRRRRK!", yell = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 54944,
        maxCount = 25,
    },
    {
        id = 12433,
        chance = 10397,
    },
    {
        id = 2671,
        chance = 9754,
    },
    {
        id = 2381,
        chance = 6943,
    },
    {
        id = 2378,
        chance = 5740,
    },
    {
        id = 3965,
        chance = 4968,
    },
    {
        id = 12435,
        chance = 4144,
    },
    {
        id = 11113,
        chance = 2763,
    },
    {
        id = 2464,
        chance = 982,
    },
    {
        id = 2044,
        chance = 840,
    },
}

mtype:register(monster)
