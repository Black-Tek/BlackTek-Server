local mtype = Game.createMonsterType("Grorlam")
local monster = {}

monster.name = "Grorlam"
monster.description = "Grorlam"
monster.experience = 2400
monster.race = "undead"
monster.maxHealth = 3000
monster.health = 3000
monster.speed = 240
monster.manaCost = 0
monster.corpse = 6005
monster.outfit = { lookType = 205 }
monster.changeTarget = {
    interval = 5000,
    chance = 3,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
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
        maxDamage = -300,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -150,
        maxDamage = -200,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 25,
    armor = 15,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 100,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 6,
        speed = 270,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 98,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 6,
    },
    {
        id = 11227,
        chance = 100000,
    },
    {
        id = 2150,
        chance = 73077,
        maxCount = 2,
    },
    {
        id = 2509,
        chance = 65385,
    },
    {
        id = 11232,
        chance = 65385,
    },
    {
        id = 2156,
        chance = 57692,
    },
    {
        id = 2166,
        chance = 42308,
    },
    {
        id = 2645,
        chance = 11538,
    },
    {
        id = 2124,
        chance = 3846,
    },
    {
        id = 1294,
        chance = 3846,
        maxCount = 3,
    },
}

mtype:register(monster)
