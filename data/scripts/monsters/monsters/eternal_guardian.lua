local mtype = Game.createMonsterType("Eternal Guardian")
local monster = {}

monster.name = "Eternal Guardian"
monster.description = "an eternal guardian"
monster.experience = 1800
monster.race = "undead"
monster.maxHealth = 2500
monster.health = 2500
monster.speed = 204
monster.manaCost = 0
monster.corpse = 11300
monster.outfit = { lookType = 345 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
}
monster.defenses = {
    defense = 62,
    armor = 62,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 70},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zzrkrrch!", yell = false},
    {text = "<crackle>", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99955,
        maxCount = 100,
    },
    {
        id = 2152,
        chance = 99708,
        maxCount = 4,
    },
    {
        id = 1294,
        chance = 30268,
        maxCount = 10,
    },
    {
        id = 10549,
        chance = 20024,
    },
    {
        id = 11325,
        chance = 10020,
    },
    {
        id = 11323,
        chance = 1991,
    },
    {
        id = 5880,
        chance = 1682,
    },
    {
        id = 11227,
        chance = 819,
    },
    {
        id = 2528,
        chance = 807,
    },
    {
        id = 11339,
        chance = 729,
    },
    {
        id = 2427,
        chance = 566,
    },
    {
        id = 11343,
        chance = 499,
    },
    {
        id = 13757,
        chance = 443,
    },
    {
        id = 11307,
        chance = 107,
    },
}

mtype:register(monster)
