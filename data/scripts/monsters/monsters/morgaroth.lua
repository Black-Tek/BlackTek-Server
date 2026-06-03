local mtype = Game.createMonsterType("Morgaroth")
local monster = {}

monster.name = "Morgaroth"
monster.description = "Morgaroth"
monster.experience = 15000
monster.race = "fire"
monster.maxHealth = 55000
monster.health = 55000
monster.speed = 400
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 1,
    lookBody = 75,
    lookLegs = 59,
    lookFeet = 1,
}
monster.changeTarget = {
    interval = 10000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 98
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
        maxDamage = -2000,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 35,
        range = 7,
        minDamage = -700,
        maxDamage = -1200,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "melee",
        interval = 1800,
        chance = 40,
        range = 7,
        minDamage = -250,
        maxDamage = -400,
        radius = 5,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "energy",
        interval = 3000,
        chance = 30,
        minDamage = -300,
        maxDamage = -1000,
        length = 8,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "physical",
        interval = 2500,
        chance = 20,
        range = 7,
        minDamage = -200,
        maxDamage = -480,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -250,
        maxDamage = -500,
        radius = 13,
        target = false,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        minDamage = -200,
        maxDamage = -450,
        radius = 14,
        target = false,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "melee",
        interval = 3000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
        radius = 3,
        target = false,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.defenses = {
    defense = 130,
    armor = 130,
    {
        name = "healing",
        interval = 3000,
        chance = 35,
        minDamage = 300,
        maxDamage = 800,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 4000,
        chance = 80,
        speed = 440,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = 80},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 80},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 6
monster.summons = {
    {name = "Demon", interval = 4000, chance = 33, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 30,
    {text = "I AM MORGAROTH, LORD OF THE TRIANGLE... AND YOU ARE LOST!", yell = true},
    {text = "MY SEED IS FEAR AND MY HARVEST ARE YOUR SOULS!", yell = true},
    {text = "ZATHROTH! LOOK AT THE DESTRUCTION I AM CAUSING IN YOUR NAME!", yell = true},
    {text = "THE TRIANGLE OF TERROR WILL RISE!", yell = true},
}
monster.loot = {
    {
        id = 6500,
        chance = 95455,
    },
    {
        id = 2152,
        chance = 95455,
        maxCount = 74,
    },
    {
        id = 2155,
        chance = 50000,
    },
    {
        id = 7590,
        chance = 45455,
    },
    {
        id = 2150,
        chance = 36364,
        maxCount = 18,
    },
    {
        id = 8852,
        chance = 36364,
    },
    {
        id = 1986,
        chance = 27273,
    },
    {
        id = 2149,
        chance = 27273,
        maxCount = 6,
    },
    {
        id = 2146,
        chance = 27273,
        maxCount = 8,
    },
    {
        id = 8473,
        chance = 27273,
    },
    {
        id = 8850,
        chance = 22727,
    },
    {
        id = 8865,
        chance = 22727,
    },
    {
        id = 6300,
        chance = 22727,
    },
    {
        id = 5954,
        chance = 22727,
        maxCount = 2,
    },
    {
        id = 2214,
        chance = 22727,
    },
    {
        id = 2151,
        chance = 22727,
        maxCount = 7,
    },
    {
        id = 8853,
        chance = 22727,
    },
    {
        id = 2387,
        chance = 18182,
    },
    {
        id = 8881,
        chance = 18182,
    },
    {
        id = 8472,
        chance = 18182,
    },
    {
        id = 2472,
        chance = 18182,
    },
    {
        id = 2164,
        chance = 18182,
    },
    {
        id = 2178,
        chance = 18182,
    },
    {
        id = 8851,
        chance = 18182,
    },
    {
        id = 2165,
        chance = 18182,
    },
    {
        id = 2112,
        chance = 18182,
    },
    {
        id = 7368,
        chance = 13636,
        maxCount = 28,
    },
    {
        id = 2144,
        chance = 13636,
        maxCount = 15,
    },
    {
        id = 7431,
        chance = 13636,
    },
    {
        id = 2033,
        chance = 13636,
    },
    {
        id = 5943,
        chance = 13636,
    },
    {
        id = 8928,
        chance = 13636,
    },
    {
        id = 8929,
        chance = 13636,
    },
    {
        id = 2143,
        chance = 13636,
        maxCount = 13,
    },
    {
        id = 2158,
        chance = 9091,
    },
    {
        id = 2520,
        chance = 9091,
    },
    {
        id = 2167,
        chance = 9091,
    },
    {
        id = 2393,
        chance = 9091,
    },
    {
        id = 2179,
        chance = 9091,
    },
    {
        id = 2470,
        chance = 9091,
    },
    {
        id = 2177,
        chance = 9091,
    },
    {
        id = 2162,
        chance = 9091,
    },
    {
        id = 2176,
        chance = 9091,
    },
    {
        id = 2645,
        chance = 9091,
    },
    {
        id = 2174,
        chance = 9091,
    },
    {
        id = 2421,
        chance = 9091,
    },
    {
        id = 2124,
        chance = 4545,
    },
    {
        id = 8867,
        chance = 4545,
    },
    {
        id = 2432,
        chance = 4545,
    },
    {
        id = 7591,
        chance = 4545,
    },
    {
        id = 2514,
        chance = 4545,
    },
    {
        id = 8886,
        chance = 4545,
    },
    {
        id = 2145,
        chance = 4545,
        maxCount = 5,
    },
}

mtype:register(monster)
