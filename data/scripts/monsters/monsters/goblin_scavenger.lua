local mtype = Game.createMonsterType("Goblin Scavenger")
local monster = {}

monster.name = "Goblin Scavenger"
monster.description = "a goblin scavenger"
monster.experience = 37
monster.race = "blood"
monster.maxHealth = 60
monster.health = 60
monster.speed = 132
monster.manaCost = 310
monster.corpse = 6002
monster.outfit = { lookType = 297 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
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
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -30,
        shootEffect = CONST_ANI_SPEAR,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -22,
        maxDamage = -30,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "death",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -1,
        maxDamage = -30,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 10,
        maxDamage = 16,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Shiny, shiny!", yell = false},
    {text = "You mean!", yell = false},
    {text = "All mine!", yell = false},
    {text = "Uhh!", yell = false},
    {text = "Gimme gimme!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 50077,
        maxCount = 9,
    },
    {
        id = 1294,
        chance = 24937,
        maxCount = 2,
    },
    {
        id = 2379,
        chance = 18190,
    },
    {
        id = 2667,
        chance = 12982,
    },
    {
        id = 2230,
        chance = 12643,
    },
    {
        id = 2461,
        chance = 9855,
    },
    {
        id = 2559,
        chance = 9448,
    },
    {
        id = 2406,
        chance = 8993,
    },
    {
        id = 2467,
        chance = 7696,
    },
    {
        id = 2235,
        chance = 7018,
    },
    {
        id = 2449,
        chance = 5044,
    },
}

mtype:register(monster)
