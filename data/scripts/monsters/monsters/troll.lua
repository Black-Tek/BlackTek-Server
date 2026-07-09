local mtype = Game.createMonsterType("Troll")
local monster = {}

monster.name = "Troll"
monster.description = "a troll"
monster.experience = 20
monster.race = "blood"
monster.maxHealth = 50
monster.health = 50
monster.speed = 126
monster.manaCost = 290
monster.corpse = 5960
monster.outfit = { lookType = 15 }
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
        maxDamage = -15,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
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
    {text = "Grrrr", yell = false},
    {text = "Groar", yell = false},
    {text = "Gruntz!", yell = false},
    {text = "Hmmm, bugs", yell = false},
    {text = "Hmmm, dogs", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 65542,
        maxCount = 12,
    },
    {
        id = 2380,
        chance = 19690,
    },
    {
        id = 2666,
        chance = 15431,
    },
    {
        id = 2389,
        chance = 12998,
    },
    {
        id = 2461,
        chance = 11892,
    },
    {
        id = 2643,
        chance = 9983,
    },
    {
        id = 2120,
        chance = 8131,
    },
    {
        id = 2448,
        chance = 5448,
    },
    {
        id = 2512,
        chance = 5448,
    },
    {
        id = 10606,
        chance = 1189,
    },
}

mtype:register(monster)
