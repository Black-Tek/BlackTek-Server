local mtype = Game.createMonsterType("Grim Reaper")
local monster = {}

monster.name = "Grim Reaper"
monster.description = "a grim reaper"
monster.experience = 5500
monster.race = "undead"
monster.maxHealth = 3900
monster.health = 3900
monster.speed = 320
monster.manaCost = 0
monster.corpse = 8955
monster.outfit = { lookType = 300 }
monster.changeTarget = {
    interval = 5000,
    chance = 20,
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -320,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -165,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -350,
        maxDamage = -720,
        length = 8,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = 0,
        maxDamage = -300,
        length = 7,
        spread = 3,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -225,
        maxDamage = -275,
        radius = 4,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 130,
        maxDamage = 205,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 450,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = 65},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 80},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Death!", yell = false},
    {text = "Come a little closer!", yell = false},
    {text = "The end is near!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99228,
        maxCount = 272,
    },
    {
        id = 6558,
        chance = 36153,
    },
    {
        id = 6500,
        chance = 11837,
    },
    {
        id = 8473,
        chance = 10839,
    },
    {
        id = 7590,
        chance = 10035,
    },
    {
        id = 2550,
        chance = 9778,
    },
    {
        id = 10577,
        chance = 5436,
    },
    {
        id = 2152,
        chance = 5146,
        maxCount = 4,
    },
    {
        id = 2162,
        chance = 4921,
    },
    {
        id = 2521,
        chance = 2830,
    },
    {
        id = 9810,
        chance = 2509,
    },
    {
        id = 5022,
        chance = 1287,
        maxCount = 4,
    },
    {
        id = 8910,
        chance = 1190,
    },
    {
        id = 7418,
        chance = 868,
    },
    {
        id = 7896,
        chance = 289,
    },
    {
        id = 8889,
        chance = 257,
    },
    {
        id = 6300,
        chance = 225,
    },
}

mtype:register(monster)
