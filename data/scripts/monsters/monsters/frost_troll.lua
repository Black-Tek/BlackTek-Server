local mtype = Game.createMonsterType("Frost Troll")
local monster = {}

monster.name = "Frost Troll"
monster.description = "a frost troll"
monster.experience = 23
monster.race = "blood"
monster.maxHealth = 55
monster.health = 55
monster.speed = 140
monster.manaCost = 300
monster.corpse = 5998
monster.outfit = { lookType = 53 }
monster.runHealth = 10
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
        maxDamage = -20,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 40},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Brrr", yell = false},
    {text = "Broar!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 50404,
        maxCount = 12,
    },
    {
        id = 2389,
        chance = 20343,
    },
    {
        id = 2667,
        chance = 18162,
    },
    {
        id = 2384,
        chance = 15591,
    },
    {
        id = 2512,
        chance = 15508,
    },
    {
        id = 2245,
        chance = 8229,
    },
    {
        id = 10565,
        chance = 2011,
    },
    {
        id = 2651,
        chance = 1126,
    },
}

mtype:register(monster)
