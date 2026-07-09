local mtype = Game.createMonsterType("Frost Dragon")
local monster = {}

monster.name = "Frost Dragon"
monster.description = "a frost dragon"
monster.experience = 2100
monster.race = "undead"
monster.maxHealth = 1800
monster.health = 1800
monster.speed = 212
monster.manaCost = 0
monster.corpse = 7091
monster.outfit = { lookType = 248 }
monster.runHealth = 250
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
    illusionable = true,
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
        maxDamage = -225,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -175,
        maxDamage = -380,
        length = 8,
        spread = 3,
        target = false,
        effect = CONST_ME_POFF,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 5,
        radius = 3,
        target = false,
        speed = -700,
        duration = 12000,
        effect = CONST_ME_POFF,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        length = 7,
        spread = 3,
        target = false,
        speed = -850,
        duration = 18000,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 5,
        minDamage = -60,
        maxDamage = -120,
        radius = 3,
        target = false,
        effect = CONST_ME_ICETORNADO,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -240,
        radius = 4,
        target = true,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 5,
        minDamage = 0,
        maxDamage = -220,
        length = 1,
        target = false,
        effect = CONST_ME_POFF,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        radius = 4,
        target = true,
        speed = -600,
        duration = 12000,
        effect = CONST_ME_ICEAREA,
    },
}
monster.defenses = {
    defense = 38,
    armor = 38,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 150,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 290,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "YOU WILL FREEZE!", yell = true},
    {text = "ZCHHHHH!", yell = true},
    {text = "I am so cool.", yell = false},
    {text = "Chill out!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 95017,
        maxCount = 243,
    },
    {
        id = 2672,
        chance = 79868,
    },
    {
        id = 2796,
        chance = 12151,
    },
    {
        id = 1976,
        chance = 8835,
    },
    {
        id = 2547,
        chance = 5895,
        maxCount = 6,
    },
    {
        id = 2146,
        chance = 5092,
    },
    {
        id = 2167,
        chance = 4952,
    },
    {
        id = 7441,
        chance = 4089,
    },
    {
        id = 2033,
        chance = 3081,
    },
    {
        id = 2177,
        chance = 608,
    },
    {
        id = 7290,
        chance = 524,
    },
    {
        id = 2479,
        chance = 453,
    },
    {
        id = 2396,
        chance = 342,
    },
    {
        id = 2528,
        chance = 291,
    },
    {
        id = 2498,
        chance = 206,
    },
    {
        id = 2492,
        chance = 111,
    },
    {
        id = 7402,
        chance = 97,
    },
}

mtype:register(monster)
