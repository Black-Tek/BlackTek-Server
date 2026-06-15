local mtype = Game.createMonsterType("Madareth")
local monster = {}

monster.name = "Madareth"
monster.description = "Madareth"
monster.experience = 10000
monster.race = "fire"
monster.maxHealth = 75000
monster.health = 75000
monster.speed = 330
monster.manaCost = 0
monster.corpse = 8721
monster.outfit = {
    lookType = 12,
    lookHead = 77,
    lookBody = 78,
    lookLegs = 100,
    lookFeet = 79,
}
monster.runHealth = 1000
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -2000,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 40,
        range = 7,
        minDamage = -180,
        maxDamage = -660,
        radius = 6,
        target = false,
        effect = CONST_ME_ENERGYAREA,
    },
    {
        name = "death",
        interval = 4000,
        chance = 30,
        minDamage = -600,
        maxDamage = -850,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -200,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 5,
        minDamage = 0,
        maxDamage = -250,
        radius = 7,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 65,
    armor = 65,
    {
        name = "healing",
        interval = 3000,
        chance = 35,
        minDamage = 300,
        maxDamage = 800,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 1},
    {type = COMBAT_DEATHDAMAGE, percent = 95},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "I am going to play with yourself!", yell = true},
    {text = "Feel my wrath!", yell = false},
    {text = "No one matches my battle prowess!", yell = false},
    {text = "You will all die!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 9813,
        chance = 59259,
    },
    {
        id = 9810,
        chance = 40741,
    },
    {
        id = 7443,
        chance = 37037,
    },
    {
        id = 7591,
        chance = 29630,
    },
    {
        id = 8472,
        chance = 27778,
    },
    {
        id = 7439,
        chance = 25926,
    },
    {
        id = 7440,
        chance = 25926,
    },
    {
        id = 7590,
        chance = 22222,
    },
    {
        id = 6300,
        chance = 20370,
    },
    {
        id = 2152,
        chance = 20370,
        maxCount = 26,
    },
    {
        id = 8473,
        chance = 20370,
    },
    {
        id = 8912,
        chance = 18519,
    },
    {
        id = 7404,
        chance = 16667,
    },
    {
        id = 2072,
        chance = 16667,
    },
    {
        id = 2377,
        chance = 16667,
    },
    {
        id = 2208,
        chance = 14815,
    },
    {
        id = 2183,
        chance = 14815,
    },
    {
        id = 7418,
        chance = 14815,
    },
    {
        id = 8910,
        chance = 14815,
    },
    {
        id = 2187,
        chance = 14815,
    },
    {
        id = 7416,
        chance = 12963,
    },
    {
        id = 2209,
        chance = 12963,
    },
    {
        id = 7407,
        chance = 12963,
    },
    {
        id = 2071,
        chance = 12963,
    },
    {
        id = 2214,
        chance = 12963,
    },
    {
        id = 8922,
        chance = 12963,
    },
    {
        id = 6500,
        chance = 11111,
    },
    {
        id = 3953,
        chance = 11111,
    },
    {
        id = 2079,
        chance = 11111,
    },
    {
        id = 7449,
        chance = 9259,
    },
    {
        id = 2168,
        chance = 9259,
    },
    {
        id = 2169,
        chance = 9259,
    },
    {
        id = 5954,
        chance = 7407,
        maxCount = 2,
    },
    {
        id = 3952,
        chance = 7407,
    },
    {
        id = 7383,
        chance = 7407,
    },
    {
        id = 8920,
        chance = 7407,
    },
    {
        id = 7386,
        chance = 5556,
    },
    {
        id = 2070,
        chance = 5556,
    },
    {
        id = 2213,
        chance = 3704,
    },
    {
        id = 2396,
        chance = 3704,
    },
    {
        id = 2207,
        chance = 3704,
    },
}

mtype:register(monster)
