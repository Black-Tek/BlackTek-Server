local mtype = Game.createMonsterType("Demon Outcast")
local monster = {}

monster.name = "Demon Outcast"
monster.description = "a demon outcast"
monster.experience = 6200
monster.race = "fire"
monster.maxHealth = 6900
monster.health = 6900
monster.speed = 296
monster.manaCost = 0
monster.corpse = 22549
monster.outfit = { lookType = 590 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -400,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -250,
        maxDamage = -450,
        length = 6,
        target = true,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -350,
        maxDamage = -550,
        length = 8,
        target = true,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        minDamage = -100,
        maxDamage = -250,
        radius = 3,
        target = true,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "demon outcast skill reducer",
        interval = 2000,
        chance = 10,
        range = 5,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -80,
        maxDamage = -150,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 250,
        maxDamage = 425,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_ENERGYDAMAGE, percent = -8},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = -6},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "energy elemental", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Back in the evil business!", yell = false},
    {text = "This prison break will have casualties!", yell = false},
    {text = "At last someone to hurt", yell = false},
    {text = "No one will imprison me again!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 6,
    },
    {
        id = 2795,
        chance = 20245,
        maxCount = 6,
    },
    {
        id = 8473,
        chance = 20029,
        maxCount = 3,
    },
    {
        id = 7590,
        chance = 19829,
        maxCount = 2,
    },
    {
        id = 2145,
        chance = 10310,
        maxCount = 5,
    },
    {
        id = 2147,
        chance = 10055,
        maxCount = 5,
    },
    {
        id = 2146,
        chance = 10003,
        maxCount = 5,
    },
    {
        id = 2149,
        chance = 9939,
        maxCount = 5,
    },
    {
        id = 9970,
        chance = 9784,
        maxCount = 5,
    },
    {
        id = 7368,
        chance = 8655,
        maxCount = 10,
    },
    {
        id = 2214,
        chance = 2448,
    },
    {
        id = 2393,
        chance = 2010,
    },
    {
        id = 2165,
        chance = 1397,
    },
    {
        id = 2519,
        chance = 903,
    },
    {
        id = 2396,
        chance = 877,
    },
    {
        id = 2164,
        chance = 661,
    },
    {
        id = 2171,
        chance = 661,
    },
    {
        id = 2497,
        chance = 571,
    },
    {
        id = 2462,
        chance = 555,
    },
    {
        id = 5906,
        chance = 532,
    },
    {
        id = 22396,
        chance = 468,
    },
    {
        id = 2520,
        chance = 458,
    },
    {
        id = 2487,
        chance = 274,
    },
    {
        id = 7382,
        chance = 87,
    },
}

mtype:register(monster)
