local mtype = Game.createMonsterType("Mazoran")
local monster = {}

monster.name = "Mazoran"
monster.description = "Mazoran"
monster.experience = 50000
monster.race = "fire"
monster.maxHealth = 290000
monster.health = 290000
monster.speed = 400
monster.manaCost = 0
monster.corpse = 25151
monster.outfit = {
    lookType = 842,
    lookHead = 97,
    lookBody = 80,
    lookLegs = 79,
    lookFeet = 0,
    lookAddons = 3,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = -1500,
        maxDamage = -2500,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 20,
        minDamage = -500,
        maxDamage = -1000,
        length = 10,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        radius = 7,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -300,
        maxDamage = -700,
        radius = 5,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        minDamage = -500,
        maxDamage = -800,
        length = 10,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        minDamage = -500,
        maxDamage = -800,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREATTACK,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 2090,
        maxDamage = 4500,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 35,
        speed = 700,
        duration = 6000,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "drunk", condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "ASHES TO ASHES, TASTE MY FIRE!", yell = false},
    {text = "BUUURN!", yell = false},
    {text = "UNLEASH THE FIRE!", yell = false},
}
monster.loot = {
    {
        id = 25172,
        chance = 100000,
    },
    {
        id = 18419,
        chance = 23000,
        maxCount = 5,
    },
    {
        id = 18420,
        chance = 23000,
        maxCount = 5,
    },
    {
        id = 18421,
        chance = 23000,
        maxCount = 5,
    },
    {
        id = 2143,
        chance = 12000,
        maxCount = 8,
    },
    {
        id = 2146,
        chance = 12000,
        maxCount = 9,
    },
    {
        id = 2148,
        chance = 49000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 49000,
        maxCount = 100,
    },
    {
        id = 2150,
        chance = 10000,
        maxCount = 5,
    },
    {
        id = 2152,
        chance = 8000,
        maxCount = 58,
    },
    {
        id = 2155,
        chance = 1000,
    },
    {
        id = 2158,
        chance = 1000,
    },
    {
        id = 2167,
        chance = 4000,
    },
    {
        id = 2432,
        chance = 3000,
    },
    {
        id = 25416,
        chance = 500,
    },
    {
        id = 2542,
        chance = 500,
    },
    {
        id = 25522,
        chance = 500,
    },
    {
        id = 25523,
        chance = 500,
    },
    {
        id = 6500,
        chance = 11000,
    },
    {
        id = 7382,
        chance = 1000,
    },
    {
        id = 7590,
        chance = 23000,
        maxCount = 5,
    },
    {
        id = 7632,
        chance = 14000,
        maxCount = 5,
    },
    {
        id = 7633,
        chance = 14000,
        maxCount = 5,
    },
    {
        id = 7890,
        chance = 1000,
    },
    {
        id = 7894,
        chance = 1000,
    },
    {
        id = 7899,
        chance = 1000,
    },
    {
        id = 8472,
        chance = 46100,
        maxCount = 5,
    },
    {
        id = 8473,
        chance = 23000,
        maxCount = 5,
    },
    {
        id = 9970,
        chance = 10000,
        maxCount = 8,
    },
    {
        id = 9971,
        chance = 3000,
    },
}

mtype:register(monster)
