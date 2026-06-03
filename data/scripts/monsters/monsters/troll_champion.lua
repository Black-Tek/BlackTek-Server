local mtype = Game.createMonsterType("Troll Champion")
local monster = {}

monster.name = "Troll Champion"
monster.description = "a troll champion"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 75
monster.health = 75
monster.speed = 190
monster.manaCost = 340
monster.corpse = 7926
monster.outfit = { lookType = 281 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
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
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Meee maity!", yell = false},
    {text = "Grrrr", yell = false},
    {text = "Whaaaz up!?", yell = false},
    {text = "Gruntz!", yell = false},
    {text = "Groar", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 64606,
        maxCount = 12,
    },
    {
        id = 2389,
        chance = 25082,
    },
    {
        id = 2666,
        chance = 9962,
    },
    {
        id = 2643,
        chance = 9936,
    },
    {
        id = 2512,
        chance = 5322,
    },
    {
        id = 2544,
        chance = 5069,
        maxCount = 5,
    },
    {
        id = 2448,
        chance = 5069,
    },
    {
        id = 10606,
        chance = 3075,
    },
    {
        id = 12471,
        chance = 909,
    },
    {
        id = 2170,
        chance = 127,
    },
}

mtype:register(monster)
