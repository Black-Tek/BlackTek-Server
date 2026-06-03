local mtype = Game.createMonsterType("Zushuka")
local monster = {}

monster.name = "Zushuka"
monster.description = "Zushuka"
monster.experience = 9000
monster.race = "blood"
monster.maxHealth = 15000
monster.health = 15000
monster.speed = 220
monster.manaCost = 0
monster.corpse = 20546
monster.outfit = {
    lookType = 149,
    lookHead = 0,
    lookBody = 9,
    lookLegs = 67,
    lookFeet = 67,
    lookAddons = 0,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
        maxDamage = -560,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -100,
        length = 8,
        spread = 3,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -110,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -300,
        maxDamage = -750,
        length = 8,
        spread = 3,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        range = 7,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -330,
        duration = 20000,
        effect = CONST_ME_ICETORNADO,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 10000,
        chance = 1,
        minDamage = 7500,
        maxDamage = 7515,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 3000,
        chance = 15,
        minDamage = 200,
        maxDamage = 500,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Cool down, will you?", yell = false},
    {text = "Freeze!", yell = false},
    {text = "Is this all you've got?", yell = false},
    {text = "Pay for your ignorance!", yell = false},
    {text = "And stay cool.", yell = false},
    {text = "Your cold dead body will be a marvelous ice statue.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 42000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 42000,
        maxCount = 100,
    },
    {
        id = 21400,
        chance = 92000,
    },
    {
        id = 7290,
        chance = 57000,
    },
    {
        id = 7449,
        chance = 42000,
    },
    {
        id = 8472,
        chance = 42000,
        maxCount = 5,
    },
    {
        id = 2396,
        chance = 42000,
    },
    {
        id = 2168,
        chance = 42000,
    },
    {
        id = 7443,
        chance = 35000,
    },
    {
        id = 7892,
        chance = 35000,
    },
    {
        id = 7440,
        chance = 35000,
    },
    {
        id = 2152,
        chance = 35000,
    },
    {
        id = 5909,
        chance = 35000,
        maxCount = 2,
    },
    {
        id = 7439,
        chance = 28000,
    },
    {
        id = 2158,
        chance = 28000,
    },
    {
        id = 2663,
        chance = 28000,
    },
    {
        id = 7888,
        chance = 21000,
    },
    {
        id = 7590,
        chance = 21000,
        maxCount = 5,
    },
    {
        id = 5912,
        chance = 14000,
    },
    {
        id = 2445,
        chance = 14000,
    },
    {
        id = 2201,
        chance = 14000,
    },
    {
        id = 7896,
        chance = 14000,
    },
    {
        id = 7897,
        chance = 14000,
    },
    {
        id = 9971,
        chance = 14000,
    },
    {
        id = 7591,
        chance = 14000,
        maxCount = 5,
    },
    {
        id = 7459,
        chance = 14000,
    },
    {
        id = 2436,
        chance = 14000,
    },
    {
        id = 2195,
        chance = 7000,
    },
    {
        id = 7902,
        chance = 7000,
    },
    {
        id = 5911,
        chance = 7000,
    },
    {
        id = 21699,
        chance = 7000,
    },
    {
        id = 21700,
        chance = 3000,
    },
    {
        id = 7410,
        chance = 3000,
    },
}

mtype:register(monster)
