local mtype = Game.createMonsterType("Annihilon")
local monster = {}

monster.name = "Annihilon"
monster.description = "Annihilon"
monster.experience = 15000
monster.race = "fire"
monster.maxHealth = 46500
monster.health = 46500
monster.speed = 380
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 3,
    lookBody = 28,
    lookLegs = 115,
    lookFeet = 39,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
        maxDamage = -1707,
    },
    {
        name = "death",
        interval = 1000,
        chance = 11,
        minDamage = 0,
        maxDamage = -600,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -200,
        maxDamage = -700,
        radius = 4,
        target = false,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 18,
        minDamage = -50,
        maxDamage = -255,
        radius = 5,
        target = true,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -50,
        maxDamage = -600,
        radius = 6,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 55,
    armor = 60,
    {
        name = "healing",
        interval = 1000,
        chance = 14,
        minDamage = 400,
        maxDamage = 900,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 4,
        speed = 500,
        duration = 7000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 95},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 95},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Flee as long as you can!", yell = false},
    {text = "Annihilon's might will crush you all!", yell = false},
    {text = "I am coming for you!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 9810,
        chance = 62097,
    },
    {
        id = 7632,
        chance = 37903,
        maxCount = 2,
    },
    {
        id = 2452,
        chance = 28226,
    },
    {
        id = 7368,
        chance = 23790,
        maxCount = 50,
    },
    {
        id = 2158,
        chance = 22581,
    },
    {
        id = 2427,
        chance = 22581,
    },
    {
        id = 2154,
        chance = 22581,
    },
    {
        id = 2127,
        chance = 22177,
    },
    {
        id = 9971,
        chance = 20968,
    },
    {
        id = 2156,
        chance = 20968,
    },
    {
        id = 6529,
        chance = 20565,
        maxCount = 46,
    },
    {
        id = 7439,
        chance = 19758,
    },
    {
        id = 2152,
        chance = 19758,
        maxCount = 30,
    },
    {
        id = 2547,
        chance = 19758,
        maxCount = 99,
    },
    {
        id = 2381,
        chance = 19355,
    },
    {
        id = 7840,
        chance = 18952,
        maxCount = 96,
    },
    {
        id = 5944,
        chance = 18952,
        maxCount = 5,
    },
    {
        id = 2153,
        chance = 17742,
    },
    {
        id = 7366,
        chance = 16935,
        maxCount = 70,
    },
    {
        id = 2155,
        chance = 16129,
    },
    {
        id = 7440,
        chance = 15323,
    },
    {
        id = 7591,
        chance = 14516,
    },
    {
        id = 7421,
        chance = 14113,
    },
    {
        id = 8473,
        chance = 13306,
    },
    {
        id = 7590,
        chance = 12903,
    },
    {
        id = 2519,
        chance = 12500,
    },
    {
        id = 8472,
        chance = 12500,
    },
    {
        id = 5954,
        chance = 11694,
        maxCount = 2,
    },
    {
        id = 8891,
        chance = 10484,
    },
    {
        id = 2528,
        chance = 10081,
    },
    {
        id = 2515,
        chance = 8468,
    },
    {
        id = 7387,
        chance = 7258,
    },
    {
        id = 2520,
        chance = 4032,
    },
    {
        id = 2514,
        chance = 3629,
    },
    {
        id = 7431,
        chance = 1613,
    },
    {
        id = 8877,
        chance = 1210,
    },
    {
        id = 8928,
        chance = 1210,
    },
    {
        id = 8929,
        chance = 403,
    },
}

mtype:register(monster)
