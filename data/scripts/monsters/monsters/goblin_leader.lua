local mtype = Game.createMonsterType("Goblin Leader")
local monster = {}

monster.name = "Goblin Leader"
monster.description = "a goblin leader"
monster.experience = 75
monster.race = "blood"
monster.maxHealth = 50
monster.health = 50
monster.speed = 120
monster.manaCost = 290
monster.corpse = 6002
monster.outfit = { lookType = 61 }
monster.runHealth = 10
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
        maxDamage = -50,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -45,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Go go, Gobo attack!!", yell = false},
    {text = "Me the greenest and the meanest!", yell = false},
    {text = "Me have power to crush you!", yell = false},
    {text = "Goblinkiller! Catch him !!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 49369,
        maxCount = 9,
    },
    {
        id = 2379,
        chance = 17498,
    },
    {
        id = 2667,
        chance = 12989,
    },
    {
        id = 2230,
        chance = 11545,
    },
    {
        id = 2559,
        chance = 10704,
    },
    {
        id = 2461,
        chance = 9862,
    },
    {
        id = 2406,
        chance = 9862,
    },
    {
        id = 2467,
        chance = 7456,
    },
    {
        id = 2235,
        chance = 7276,
    },
    {
        id = 2449,
        chance = 4871,
    },
}

mtype:register(monster)
