local mtype = Game.createMonsterType("Lich")
local monster = {}

monster.name = "Lich"
monster.description = "a lich"
monster.experience = 900
monster.race = "undead"
monster.maxHealth = 880
monster.health = 880
monster.speed = 210
monster.manaCost = 0
monster.corpse = 6028
monster.outfit = { lookType = 99 }
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
    canPushCreatures = true,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -75,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -140,
        maxDamage = -190,
        length = 7,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -300,
        maxDamage = -400,
        length = 7,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -200,
        maxDamage = -245,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -300,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -195,
        radius = 3,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 50,
    armor = 50,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 80,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 80},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Bonebeast", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Doomed be the living!", yell = false},
    {text = "Death awaits all!", yell = false},
    {text = "Thy living flesh offends me!", yell = false},
    {text = "Death and Decay!", yell = false},
    {text = "You will endure agony beyond thy death!", yell = false},
    {text = "Pain sweet pain!", yell = false},
    {text = "Come to me my children!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 139,
    },
    {
        id = 2152,
        chance = 19628,
    },
    {
        id = 2175,
        chance = 10351,
    },
    {
        id = 7589,
        chance = 7810,
    },
    {
        id = 2144,
        chance = 5252,
        maxCount = 3,
    },
    {
        id = 2143,
        chance = 4886,
    },
    {
        id = 9970,
        chance = 2439,
        maxCount = 3,
    },
    {
        id = 2149,
        chance = 2370,
        maxCount = 3,
    },
    {
        id = 2532,
        chance = 2072,
    },
    {
        id = 2214,
        chance = 1637,
    },
    {
        id = 2154,
        chance = 759,
    },
    {
        id = 2479,
        chance = 716,
    },
    {
        id = 2178,
        chance = 418,
    },
    {
        id = 2171,
        chance = 401,
    },
    {
        id = 7893,
        chance = 281,
    },
    {
        id = 2436,
        chance = 273,
    },
    {
        id = 2535,
        chance = 264,
    },
    {
        id = 2656,
        chance = 205,
    },
    {
        id = 13291,
        chance = 43,
    },
}

mtype:register(monster)
