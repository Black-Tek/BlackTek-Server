local mtype = Game.createMonsterType("Earth Overlord")
local monster = {}

monster.name = "Earth Overlord"
monster.description = "an Earth Overlord"
monster.experience = 2800
monster.race = "undead"
monster.maxHealth = 4000
monster.health = 4000
monster.speed = 330
monster.manaCost = 0
monster.corpse = 8933
monster.outfit = { lookType = 285 }
monster.changeTarget = {
    interval = 20000,
    chance = 30,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -400,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 10,
        minDamage = 0,
        maxDamage = -800,
        length = 7,
        effect = CONST_ME_STONES,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 9,
        minDamage = 0,
        maxDamage = -490,
        radius = 6,
        effect = CONST_ME_BIGPLANTS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        speed = -750,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 68,
    },
    {
        id = "platinum coin",
        chance = 33333,
        maxCount = 3,
    },
    {
        id = "terra mantle",
        chance = 1923,
    },
    {
        id = "mother soil",
        chance = 100000,
    },
    {
        id = "lump of earth",
        chance = 33333,
    },
    {
        id = "shiny stone",
        chance = 8333,
    },
    {
        id = 13757,
        chance = 552,
    },
}

mtype:register(monster)
