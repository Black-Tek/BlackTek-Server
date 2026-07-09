local mtype = Game.createMonsterType("Massive Earth Elemental")
local monster = {}

monster.name = "Massive Earth Elemental"
monster.description = "a massive earth elemental"
monster.experience = 950
monster.race = "undead"
monster.maxHealth = 1330
monster.health = 1330
monster.speed = 240
monster.manaCost = 0
monster.corpse = 8933
monster.outfit = { lookType = 285 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -99,
        maxDamage = -145,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -95,
        maxDamage = -169,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_POFF,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -300,
        maxDamage = -320,
        length = 6,
        effect = CONST_ME_BIGPLANTS,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -220,
        radius = 5,
        target = false,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 5,
        target = true,
        speed = -330,
        duration = 5000,
        effect = CONST_ME_SMALLPLANTS,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 150,
        maxDamage = 180,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 45},
    {type = COMBAT_ENERGYDAMAGE, percent = 90},
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.loot = {
    {
        id = "small stone",
        chance = 25280,
        maxCount = 10,
    },
    {
        id = "gold coin",
        chance = 32000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 32000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 32000,
        maxCount = 41,
    },
    {
        id = "stone skin amulet",
        chance = 980,
    },
    {
        id = "protection amulet",
        chance = 1580,
    },
    {
        id = "dwarven ring",
        chance = 2790,
    },
    {
        id = "diamond sceptre",
        chance = 150,
    },
    {
        id = "terra amulet",
        chance = 500,
    },
    {
        id = 9809,
        chance = 3300,
    },
    {
        id = "small topaz",
        chance = 5280,
        maxCount = 2,
    },
    {
        id = "lump of earth",
        chance = 40680,
    },
    {
        id = "clay lump",
        chance = 480,
    },
    {
        id = 13757,
        chance = 430,
    },
}

mtype:register(monster)
