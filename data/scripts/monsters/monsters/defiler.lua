local mtype = Game.createMonsterType("Defiler")
local monster = {}

monster.name = "Defiler"
monster.description = "a defiler"
monster.experience = 3700
monster.race = "venom"
monster.maxHealth = 3650
monster.health = 3650
monster.speed = 160
monster.manaCost = 0
monster.corpse = 6532
monster.outfit = { lookType = 238 }
monster.runHealth = 85
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -240,
        -- NOTE: melee condition (poison=150); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 150000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -160,
        maxDamage = -270,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -400,
        maxDamage = -640,
        radius = 7,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = -120,
        maxDamage = -170,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -500,
        maxDamage = -1000,
        length = 8,
        spread = 3,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        length = 8,
        spread = 3,
        speed = -700,
        duration = 15000,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 280,
        maxDamage = 350,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blubb", yell = false},
    {text = "Blubb Blubb", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 272,
    },
    {
        id = 2152,
        chance = 96687,
        maxCount = 6,
    },
    {
        id = 5944,
        chance = 19478,
    },
    {
        id = 6500,
        chance = 19157,
    },
    {
        id = 9967,
        chance = 11586,
    },
    {
        id = 9968,
        chance = 11245,
    },
    {
        id = 2149,
        chance = 5723,
        maxCount = 3,
    },
    {
        id = 2151,
        chance = 4357,
    },
    {
        id = 2147,
        chance = 3695,
        maxCount = 2,
    },
    {
        id = 6300,
        chance = 2831,
    },
    {
        id = 2145,
        chance = 2490,
        maxCount = 2,
    },
    {
        id = 2154,
        chance = 1787,
    },
    {
        id = 2156,
        chance = 1165,
    },
    {
        id = 2158,
        chance = 843,
    },
    {
        id = 2155,
        chance = 723,
    },
}

mtype:register(monster)
