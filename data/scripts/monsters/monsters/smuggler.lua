local mtype = Game.createMonsterType("Smuggler")
local monster = {}

monster.name = "Smuggler"
monster.description = "a smuggler"
monster.experience = 48
monster.race = "blood"
monster.maxHealth = 130
monster.health = 130
monster.speed = 176
monster.manaCost = 390
monster.corpse = 20507
monster.outfit = {
    lookType = 134,
    lookHead = 95,
    lookBody = 0,
    lookLegs = 113,
    lookFeet = 115,
}
monster.runHealth = 15
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
        maxDamage = -60,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I will silence you forever!", yell = false},
    {text = "You saw something you shouldn't!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 79815,
        maxCount = 10,
    },
    {
        id = 2050,
        chance = 30203,
        maxCount = 2,
    },
    {
        id = 2649,
        chance = 14782,
    },
    {
        id = 2671,
        chance = 10185,
    },
    {
        id = 2406,
        chance = 10051,
    },
    {
        id = 2461,
        chance = 10036,
    },
    {
        id = 2403,
        chance = 10012,
    },
    {
        id = 2376,
        chance = 5012,
    },
    {
        id = 8840,
        chance = 4896,
        maxCount = 5,
    },
    {
        id = 2404,
        chance = 4042,
    },
    {
        id = 7397,
        chance = 112,
    },
}

mtype:register(monster)
