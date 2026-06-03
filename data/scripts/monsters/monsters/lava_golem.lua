local mtype = Game.createMonsterType("Lava Golem")
local monster = {}

monster.name = "Lava Golem"
monster.description = "a lava golem"
monster.experience = 6200
monster.race = "fire"
monster.maxHealth = 9000
monster.health = 9000
monster.speed = 420
monster.manaCost = 0
monster.corpse = 17333
monster.outfit = { lookType = 491 }
monster.changeTarget = {
    interval = 5000,
    chance = 5,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    attackable = true,
    hostile = true,
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
        maxDamage = -400,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -350,
        maxDamage = -700,
        length = 8,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -600,
        maxDamage = -1300,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "lava golem soulfire",
        interval = 2000,
        chance = 15,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -220,
        maxDamage = -350,
        radius = 4,
        target = true,
        effect = CONST_ME_FIREAREA,
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
    {
        name = "fire",
        interval = 2000,
        chance = 30,
        minDamage = -280,
        maxDamage = -350,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYFIRE,
    },
}
monster.defenses = {
    defense = 84,
    armor = 84,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 450,
        duration = 7000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 35},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrrunt", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 200,
    },
    {
        id = 2152,
        chance = 99981,
        maxCount = 11,
    },
    {
        id = 7620,
        chance = 20316,
        maxCount = 2,
    },
    {
        id = 7588,
        chance = 18903,
        maxCount = 2,
    },
    {
        id = 7589,
        chance = 17770,
        maxCount = 2,
    },
    {
        id = 7590,
        chance = 16171,
        maxCount = 2,
    },
    {
        id = 18424,
        chance = 15186,
        maxCount = 2,
    },
    {
        id = 8473,
        chance = 15093,
    },
    {
        id = 18425,
        chance = 14591,
    },
    {
        id = 18416,
        chance = 13885,
        maxCount = 2,
    },
    {
        id = 10553,
        chance = 13606,
    },
    {
        id = 18435,
        chance = 12063,
        maxCount = 5,
    },
    {
        id = 5880,
        chance = 11617,
    },
    {
        id = 18420,
        chance = 10446,
    },
    {
        id = 5914,
        chance = 7175,
    },
    {
        id = 2154,
        chance = 6413,
    },
    {
        id = 18414,
        chance = 6041,
    },
    {
        id = 5909,
        chance = 4907,
    },
    {
        id = 5911,
        chance = 3569,
    },
    {
        id = 7890,
        chance = 3197,
    },
    {
        id = 2187,
        chance = 3160,
    },
    {
        id = 7891,
        chance = 2602,
    },
    {
        id = 2392,
        chance = 1784,
    },
    {
        id = 2432,
        chance = 1413,
    },
    {
        id = 18409,
        chance = 1338,
    },
    {
        id = 2519,
        chance = 1227,
    },
    {
        id = 2156,
        chance = 1171,
    },
    {
        id = 8902,
        chance = 613,
    },
    {
        id = 7899,
        chance = 428,
    },
}

mtype:register(monster)
