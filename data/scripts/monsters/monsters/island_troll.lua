local mtype = Game.createMonsterType("Island Troll")
local monster = {}

monster.name = "Island Troll"
monster.description = "an island troll"
monster.experience = 20
monster.race = "blood"
monster.maxHealth = 50
monster.health = 50
monster.speed = 126
monster.manaCost = 290
monster.corpse = 7930
monster.outfit = { lookType = 282 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 5000,
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
        maxDamage = -10,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hmmm, turtles", yell = false},
    {text = "Hmmm, dogs", yell = false},
    {text = "Hmmm, worms", yell = false},
    {text = "Groar", yell = false},
    {text = "Gruntz!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 59656,
        maxCount = 10,
    },
    {
        id = 5901,
        chance = 30727,
    },
    {
        id = 2389,
        chance = 20007,
    },
    {
        id = 2380,
        chance = 17701,
    },
    {
        id = 2512,
        chance = 15244,
    },
    {
        id = 2643,
        chance = 9794,
    },
    {
        id = 2461,
        chance = 9670,
    },
    {
        id = 2120,
        chance = 7889,
    },
    {
        id = 2448,
        chance = 5182,
    },
    {
        id = 5097,
        chance = 5102,
    },
    {
        id = 7963,
        chance = 71,
    },
    {
        id = 2170,
        chance = 71,
    },
}

mtype:register(monster)
