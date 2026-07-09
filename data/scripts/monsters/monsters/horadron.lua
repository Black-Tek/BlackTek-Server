local mtype = Game.createMonsterType("Horadron")
local monster = {}

monster.name = "Horadron"
monster.description = "Horadron"
monster.experience = 18000
monster.race = "fire"
monster.maxHealth = 30000
monster.health = 30000
monster.speed = 320
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 78,
    lookBody = 61,
    lookLegs = 115,
    lookFeet = 58,
    lookAddons = 0,
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
        skill = 110,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 25,
        minDamage = -600,
        maxDamage = -1000,
        length = 8,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 25,
        minDamage = -600,
        maxDamage = -1000,
        length = 8,
        spread = 3,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = -100,
        maxDamage = -235,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 25,
        minDamage = -100,
        maxDamage = -250,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 25,
        minDamage = -300,
        maxDamage = -450,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.defenses = {
    defense = 65,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 35,
        minDamage = 400,
        maxDamage = 700,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "poison", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Defiler", interval = 2000, chance = 12, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Even I fear the wrath of the princes. And the cold touch of those whom they serve! You are not prepared!", yell = false},
    {text = "You foolish mortals with you medding you brought the end upon your world!", yell = false},
    {text = "After all those aeons I smell freedom at last!", yell = false},
}
monster.loot = {
    {
        id = 22396,
        chance = 100000,
    },
    {
        id = 5954,
        chance = 100000,
    },
    {
        id = 6500,
        chance = 100000,
    },
    {
        id = 22397,
        chance = 13850,
    },
    {
        id = 22610,
        chance = 5380,
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
        id = 22598,
        chance = 100000,
    },
    {
        id = 8903,
        chance = 5130,
    },
    {
        id = 8901,
        chance = 35900,
    },
    {
        id = 8902,
        chance = 10260,
    },
    {
        id = 3962,
        chance = 33330,
    },
    {
        id = 7456,
        chance = 12820,
    },
    {
        id = 7453,
        chance = 2560,
    },
    {
        id = 7388,
        chance = 2560,
    },
    {
        id = 2466,
        chance = 1220,
    },
    {
        id = 2656,
        chance = 1220,
    },
    {
        id = 2487,
        chance = 17950,
    },
    {
        id = 2470,
        chance = 250,
    },
    {
        id = 2415,
        chance = 90,
    },
    {
        id = 8926,
        chance = 90,
    },
    {
        id = 7895,
        chance = 1750,
    },
    {
        id = 2477,
        chance = 1750,
    },
    {
        id = 2488,
        chance = 20510,
    },
    {
        id = 2645,
        chance = 15380,
    },
    {
        id = 2145,
        chance = 17950,
        maxCount = 25,
    },
    {
        id = 2150,
        chance = 5130,
        maxCount = 25,
    },
    {
        id = 2149,
        chance = 10260,
        maxCount = 25,
    },
    {
        id = 2146,
        chance = 28210,
        maxCount = 25,
    },
    {
        id = 9970,
        chance = 15380,
        maxCount = 25,
    },
    {
        id = 2147,
        chance = 20510,
        maxCount = 25,
    },
}

mtype:register(monster)
