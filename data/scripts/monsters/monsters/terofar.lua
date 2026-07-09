local mtype = Game.createMonsterType("Terofar")
local monster = {}

monster.name = "Terofar"
monster.description = "Terofar"
monster.experience = 24000
monster.race = "fire"
monster.maxHealth = 40000
monster.health = 40000
monster.speed = 440
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 19,
    lookBody = 0,
    lookLegs = 77,
    lookFeet = 79,
}
monster.changeTarget = {
    interval = 2000,
    chance = 25,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 100,
        skill = 180,
    },
    {
        name = "death",
        interval = 2000,
        chance = 30,
        minDamage = -700,
        maxDamage = -1500,
        length = 8,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 18,
        range = 7,
        minDamage = 0,
        maxDamage = -750,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
    },
}
monster.defenses = {
    defense = 65,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 300,
        maxDamage = 500,
        radius = 8,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 18,
        speed = 784,
        duration = 7000,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "poison", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "IT TOOK AN ARMY OF YOUR KIND TO DEFEAT THE WARDEN. NOW YOU ARE ALONE!", yell = true},
    {text = "FEELS GOOD TO BE BACK IN ACTION!", yell = true},
    {text = "THE WARDS ARE FAILING! MY ESCAPE IS ONLY A MATTER OF TIME!!", yell = true},
    {text = "Terofar cast a greater death curse on you!", yell = true},
}
monster.loot = {
    {
        id = 22396,
        chance = 100000,
    },
    {
        id = 22598,
        chance = 100000,
    },
    {
        id = 22397,
        chance = 100000,
    },
    {
        id = 6500,
        chance = 100000,
    },
    {
        id = 5954,
        chance = 100000,
    },
    {
        id = 22610,
        chance = 3560,
    },
    {
        id = 2148,
        chance = 50000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 50000,
        maxCount = 100,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 50,
    },
    {
        id = 7590,
        chance = 33330,
        maxCount = 10,
    },
    {
        id = 8473,
        chance = 3890,
        maxCount = 10,
    },
    {
        id = 8472,
        chance = 2780,
        maxCount = 5,
    },
    {
        id = 18413,
        chance = 22220,
        maxCount = 8,
    },
    {
        id = 18414,
        chance = 33330,
        maxCount = 8,
    },
    {
        id = 18415,
        chance = 44440,
        maxCount = 8,
    },
    {
        id = 9971,
        chance = 16670,
    },
    {
        id = 7632,
        chance = 38890,
    },
    {
        id = 7633,
        chance = 38890,
    },
    {
        id = 2520,
        chance = 11110,
    },
    {
        id = 2515,
        chance = 22220,
    },
    {
        id = 2519,
        chance = 11110,
    },
    {
        id = 2514,
        chance = 960,
    },
    {
        id = 8891,
        chance = 5560,
    },
    {
        id = 8879,
        chance = 11110,
    },
    {
        id = 8877,
        chance = 6110,
    },
    {
        id = 2155,
        chance = 16670,
    },
    {
        id = 2158,
        chance = 11110,
    },
    {
        id = 8882,
        chance = 410,
    },
    {
        id = 2472,
        chance = 610,
    },
    {
        id = 2645,
        chance = 910,
    },
    {
        id = 2498,
        chance = 910,
    },
    {
        id = 7886,
        chance = 2910,
    },
    {
        id = 2195,
        chance = 1910,
    },
    {
        id = 5741,
        chance = 1910,
    },
}

mtype:register(monster)
