local mtype = Game.createMonsterType("Sight Of Surrender")
local monster = {}

monster.name = "Sight Of Surrender"
monster.description = "a sight of surrender"
monster.experience = 17000
monster.race = "undead"
monster.maxHealth = 28000
monster.health = 28000
monster.speed = 340
monster.manaCost = 0
monster.corpse = 22478
monster.outfit = { lookType = 583 }
monster.changeTarget = {
    interval = 4000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1100,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        minDamage = -500,
        maxDamage = -800,
        length = 8,
        target = true,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -500,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 70,
    armor = 70,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 550,
        maxDamage = 1100,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 520,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 40},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 35},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "BOW LOW!", yell = false},
    {text = "FEEL THE TRUE MEANING OF VANQUISH!", yell = false},
    {text = "HAHAHAHA DO YOU WANT TO AMUSE YOUR MASTER?", yell = false},
    {text = "NOW YOU WILL SURRENDER!", yell = false},
}
monster.loot = {
    {
        id = 22518,
        chance = 100000,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 20,
    },
    {
        id = 22517,
        chance = 100000,
    },
    {
        id = 8473,
        chance = 76987,
        maxCount = 5,
    },
    {
        id = 7590,
        chance = 76407,
        maxCount = 5,
    },
    {
        id = 8472,
        chance = 75442,
        maxCount = 5,
    },
    {
        id = 18418,
        chance = 35099,
        maxCount = 5,
    },
    {
        id = 18416,
        chance = 32754,
        maxCount = 5,
    },
    {
        id = 18417,
        chance = 32147,
        maxCount = 5,
    },
    {
        id = 18413,
        chance = 25855,
        maxCount = 3,
    },
    {
        id = 18414,
        chance = 24558,
        maxCount = 3,
    },
    {
        id = 18415,
        chance = 23841,
        maxCount = 3,
    },
    {
        id = 2197,
        chance = 14790,
    },
    {
        id = 2445,
        chance = 5215,
    },
    {
        id = 2164,
        chance = 5188,
    },
    {
        id = 22396,
        chance = 2953,
        maxCount = 2,
    },
    {
        id = 2645,
        chance = 2511,
    },
    {
        id = 2472,
        chance = 1821,
    },
    {
        id = 7421,
        chance = 1325,
    },
    {
        id = 7422,
        chance = 1269,
    },
    {
        id = 2528,
        chance = 1269,
    },
    {
        id = 2444,
        chance = 883,
    },
    {
        id = 2497,
        chance = 800,
    },
    {
        id = 2488,
        chance = 552,
    },
    {
        id = 22542,
        chance = 386,
    },
}

mtype:register(monster)
