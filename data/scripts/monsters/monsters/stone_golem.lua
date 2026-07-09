local mtype = Game.createMonsterType("Stone Golem")
local monster = {}

monster.name = "Stone Golem"
monster.description = "a stone golem"
monster.experience = 160
monster.race = "undead"
monster.maxHealth = 270
monster.health = 270
monster.speed = 180
monster.manaCost = 590
monster.corpse = 6005
monster.outfit = { lookType = 67 }
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
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
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 95794,
        maxCount = 40,
    },
    {
        id = 1294,
        chance = 12849,
        maxCount = 4,
    },
    {
        id = 11232,
        chance = 10115,
    },
    {
        id = 2166,
        chance = 5137,
    },
    {
        id = 2395,
        chance = 2572,
    },
    {
        id = 5880,
        chance = 2054,
    },
    {
        id = 10549,
        chance = 1017,
    },
    {
        id = 11227,
        chance = 782,
    },
    {
        id = 13757,
        chance = 528,
    },
    {
        id = 11343,
        chance = 416,
    },
    {
        id = 2124,
        chance = 181,
    },
    {
        id = 2156,
        chance = 48,
    },
}

mtype:register(monster)
