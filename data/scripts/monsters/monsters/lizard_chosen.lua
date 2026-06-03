local mtype = Game.createMonsterType("Lizard Chosen")
local monster = {}

monster.name = "Lizard Chosen"
monster.description = "a lizard chosen"
monster.experience = 2200
monster.race = "blood"
monster.maxHealth = 3050
monster.health = 3050
monster.speed = 272
monster.manaCost = 0
monster.corpse = 11288
monster.outfit = { lookType = 344 }
monster.runHealth = 50
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -360,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -240,
        maxDamage = -320,
        length = 3,
        spread = 2,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -190,
        maxDamage = -340,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -90,
        maxDamage = -180,
        length = 8,
        effect = CONST_ME_GREEN_RINGS,
    },
}
monster.defenses = {
    defense = 28,
    armor = 28,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 75,
        maxDamage = 125,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grzzzzzzz!", yell = false},
    {text = "Garrrblarrrrzzzz!", yell = false},
    {text = "Kzzzzzz!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 97523,
        maxCount = 236,
    },
    {
        id = 11325,
        chance = 10048,
    },
    {
        id = 7591,
        chance = 6895,
        maxCount = 3,
    },
    {
        id = 11327,
        chance = 5939,
    },
    {
        id = 2152,
        chance = 2997,
        maxCount = 5,
    },
    {
        id = 12629,
        chance = 2995,
    },
    {
        id = 11326,
        chance = 2971,
    },
    {
        id = 2145,
        chance = 2537,
        maxCount = 5,
    },
    {
        id = 5876,
        chance = 2003,
    },
    {
        id = 2528,
        chance = 1118,
    },
    {
        id = 5881,
        chance = 1035,
    },
    {
        id = 11301,
        chance = 992,
    },
    {
        id = 11304,
        chance = 889,
    },
    {
        id = 11303,
        chance = 809,
    },
    {
        id = 11302,
        chance = 130,
    },
}

mtype:register(monster)
