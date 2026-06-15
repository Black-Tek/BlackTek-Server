local mtype = Game.createMonsterType("Ironblight")
local monster = {}

monster.name = "Ironblight"
monster.description = "Ironblight"
monster.experience = 4400
monster.race = "undead"
monster.maxHealth = 6600
monster.health = 6600
monster.speed = 286
monster.manaCost = 0
monster.corpse = 17424
monster.outfit = { lookType = 498 }
monster.runHealth = 260
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
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -460,
        maxDamage = -480,
        radius = 6,
        target = false,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 15,
        minDamage = -260,
        maxDamage = -350,
        length = 7,
        target = false,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = -180,
        maxDamage = -250,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_GREENSTAR,
        effect = CONST_ME_BIGPLANTS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        length = 5,
        spread = 3,
        target = false,
        speed = -800,
        duration = 30000,
        effect = CONST_ME_BLOCKHIT,
    },
}
monster.defenses = {
    defense = 84,
    armor = 84,
    {
        name = "invisible",
        interval = 2000,
        chance = 20,
        duration = 7000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_FIREDAMAGE, percent = 60},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Clonk!", yell = false},
    {text = "Yowl!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 199,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 8,
    },
    {
        id = 10571,
        chance = 21215,
    },
    {
        id = 8473,
        chance = 18606,
    },
    {
        id = 18432,
        chance = 17836,
    },
    {
        id = 7590,
        chance = 17151,
    },
    {
        id = 2150,
        chance = 15141,
        maxCount = 3,
    },
    {
        id = 11227,
        chance = 14286,
    },
    {
        id = 18417,
        chance = 12703,
        maxCount = 2,
    },
    {
        id = 18420,
        chance = 10736,
    },
    {
        id = 2149,
        chance = 10565,
        maxCount = 3,
    },
    {
        id = 18415,
        chance = 5518,
    },
    {
        id = 9942,
        chance = 4962,
    },
    {
        id = 2156,
        chance = 3464,
    },
    {
        id = 9980,
        chance = 2352,
    },
    {
        id = 8912,
        chance = 1668,
    },
    {
        id = 18412,
        chance = 898,
    },
    {
        id = 5904,
        chance = 898,
    },
    {
        id = 7885,
        chance = 898,
    },
    {
        id = 7437,
        chance = 770,
    },
    {
        id = 2158,
        chance = 599,
    },
    {
        id = 2438,
        chance = 599,
    },
    {
        id = 2445,
        chance = 556,
    },
    {
        id = 11368,
        chance = 257,
    },
    {
        id = 8855,
        chance = 171,
    },
}

mtype:register(monster)
