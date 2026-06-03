local mtype = Game.createMonsterType("Earth Elemental")
local monster = {}

monster.name = "Earth Elemental"
monster.description = "an earth elemental"
monster.experience = 450
monster.race = "undead"
monster.maxHealth = 650
monster.health = 650
monster.speed = 220
monster.manaCost = 0
monster.corpse = 8933
monster.outfit = { lookType = 301 }
monster.runHealth = 80
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
        minDamage = -72,
        maxDamage = -105,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_POFF,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 20,
        minDamage = -200,
        maxDamage = -260,
        length = 6,
        effect = CONST_ME_BIGPLANTS,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -140,
        radius = 5,
        target = false,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 5,
        target = true,
        speed = -330,
        duration = 5000,
        effect = CONST_ME_SMALLPLANTS,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 60,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 85},
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
}
monster.loot = {
    {
        id = "small stone",
        chance = 10000,
        maxCount = 10,
    },
    {
        id = "gold coin",
        chance = 43000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 30,
    },
    {
        id = "blank rune",
        chance = 10000,
    },
    {
        id = "strong mana potion",
        chance = 1910,
    },
    {
        id = "earth arrow",
        chance = 20160,
        maxCount = 30,
    },
    {
        id = 9808,
        chance = 350,
    },
    {
        id = "small topaz",
        chance = 620,
    },
    {
        id = "lump of earth",
        chance = 20460,
    },
    {
        id = "clay lump",
        chance = 570,
    },
    {
        id = 13757,
        chance = 470,
    },
}

mtype:register(monster)
